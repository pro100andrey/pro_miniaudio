#include "../include/context.h"

#include <stdlib.h>
#include <string.h>

#include "../include/context_private.h"
#include "../include/internal.h"
#include "../include/logger.h"
#include "../include/miniaudio.h"

void _onLog(void *pUserData, ma_uint32 level, const char *pMessage) {
    (void)pUserData;

    switch (level) {
        case MA_LOG_LEVEL_DEBUG:
            LOG_DEBUG("%s", pMessage);
            break;
        case MA_LOG_LEVEL_INFO:
            LOG_INFO("%s", pMessage);
            break;
        case MA_LOG_LEVEL_WARNING:
            LOG_WARN("%s", pMessage);
            break;
        case MA_LOG_LEVEL_ERROR:
            LOG_ERROR("%s", pMessage);
            break;
        default:
            break;
    }
}

FFI_PLUGIN_EXPORT
void *context_create(void) {
    context_t *context = malloc(sizeof(context_t));

    if (!context) {
        LOG_ERROR("failed to allocate memory for context.\n", "");
        return NULL;
    }

    context->audioDevices = NULL;
    context->deviceCount = 0;

    ma_context_config config = ma_context_config_init();
    config.coreaudio.sessionCategory =
        ma_ios_session_category_play_and_record;
    config.coreaudio.sessionCategoryOptions =
        ma_ios_session_category_option_allow_bluetooth |
        ma_ios_session_category_option_allow_bluetooth_a2dp |
        ma_ios_session_category_option_allow_air_play |
        ma_ios_session_category_option_mix_with_others;

    ma_log *log = malloc(sizeof(ma_log));
    if (ma_log_init(
            &context->maContext.allocationCallbacks,
            log) == MA_SUCCESS) {
        ma_log_callback log_callback;
        log_callback.onLog = _onLog;
        ma_log_register_callback(log, log_callback);
        config.pLog = log;
    }

    ma_result contextInitResult =
        ma_context_init(NULL,
                        0,
                        &config,
                        &context->maContext);

    if (contextInitResult != MA_SUCCESS) {
        free(context);

        LOG_ERROR("ma_context_init failed - %s.\n",
                  ma_result_description(contextInitResult));

        return NULL;
    }

    context->playbackCache.count = 0;
    context->captureCache.count = 0;

    LOG_INFO("<%p>(context_t *) created.\n", context);

    return context;
}

FFI_PLUGIN_EXPORT
void context_destroy(void *pContext) {
    if (!pContext) {
        LOG_ERROR("invalid parameter: `pContext` is NULL.\n", "");
        return;
    }

    context_t *context = (context_t *)pContext;
    // Stop and free each device
    for (size_t i = 0; i < context->deviceCount; i++) {
        audio_device_t *device = context->audioDevices[i];
        if (device->vtable && device->vtable->destroy) {
            LOG_INFO("Destroying device <%p>(audio_device_t *)\n", device);
            device->vtable->destroy(device);
        }
    }

    ma_result contextUninitResult =
        ma_context_uninit(&context->maContext);

    if (contextUninitResult != MA_SUCCESS) {
        LOG_ERROR("ma_context_uninit failed - %s.\n",
                  ma_result_description(contextUninitResult));
    }

    if (context->playbackCache.deviceInfo) {
        free(context->playbackCache.deviceInfo);
    }

    if (context->captureCache.deviceInfo) {
        free(context->captureCache.deviceInfo);
    }

    free(context->audioDevices);
    free(context->maContext.pLog);
    free(context);

    LOG_INFO("<%p>(context_t *) destroyed.\n", context);
}

static void process_device_list(context_t *pContext,
                                ma_device_type deviceType,
                                ma_device_info *pMaDeviceInfos,
                                uint32_t deviceCount,
                                device_info_cache_t *pDevicesCache) {
    // Free previous devices
    if (pDevicesCache->deviceInfo) {
        for (uint32_t i = 0; i < pDevicesCache->count; i++) {
            free(pDevicesCache->deviceInfo[i].id);
        }

        free(pDevicesCache->deviceInfo);
    }

    pDevicesCache->count = deviceCount;
    pDevicesCache->deviceInfo =
        (device_info_t *)malloc(deviceCount * sizeof(device_info_t));

    for (uint32_t i = 0; i < deviceCount; i++) {
        ma_result getDeviceInfoResult =
            ma_context_get_device_info(&pContext->maContext,
                                       deviceType,
                                       &pMaDeviceInfos[i].id,
                                       &pMaDeviceInfos[i]);

        if (getDeviceInfoResult != MA_SUCCESS) {
            LOG_ERROR("failed to get device info for %s device - %s.\n",
                      deviceType == ma_device_type_playback ? "playback" : "capture",
                      ma_result_description(getDeviceInfoResult));

            continue;
        }

        size_t nameLength = MA_MAX_DEVICE_NAME_LENGTH + 1;
        strncpy(pDevicesCache->deviceInfo[i].name, pMaDeviceInfos[i].name, nameLength);

        pDevicesCache->deviceInfo[i].id = (ma_device_id *)malloc(sizeof(ma_device_id));

        if (!pDevicesCache->deviceInfo[i].id) {
            LOG_ERROR("failed to allocate memory for device id.\n", "");
            continue;
        }

        memcpy(pDevicesCache->deviceInfo[i].id, &pMaDeviceInfos[i].id, sizeof(ma_device_id));

        pDevicesCache->deviceInfo[i].isDefault = pMaDeviceInfos[i].isDefault;
        pDevicesCache->deviceInfo[i].formatCount = pMaDeviceInfos[i].nativeDataFormatCount;

        for (uint32_t j = 0; j < pMaDeviceInfos[i].nativeDataFormatCount; j++) {
            pDevicesCache->deviceInfo[i].audioFormats[j].pcmFormat =
                (pcm_format_t)pMaDeviceInfos[i].nativeDataFormats[j].format;
            pDevicesCache->deviceInfo[i].audioFormats[j].channels =
                pMaDeviceInfos[i].nativeDataFormats[j].channels;
            pDevicesCache->deviceInfo[i].audioFormats[j].sampleRate =
                pMaDeviceInfos[i].nativeDataFormats[j].sampleRate;
        }
    }
}

FFI_PLUGIN_EXPORT
void context_refresh_devices(const void *pContext) {
    if (!pContext) {
        LOG_ERROR("invalid parameter: `pContext` is NULL.\n", "");
        return;
    }

    ma_device_info *playbackDeviceInfos;
    ma_uint32 playbackDeviceCount;

    ma_device_info *captureDeviceInfos;
    ma_uint32 captureDeviceCount;

    context_t *context = (context_t *)pContext;

    LOG_INFO("Using context: <%p>.\n", context);

    // Get playback and capture devices
    ma_result getDevicesResult =
        ma_context_get_devices(&context->maContext,
                               &playbackDeviceInfos,
                               &playbackDeviceCount,
                               &captureDeviceInfos,
                               &captureDeviceCount);

    if (getDevicesResult != MA_SUCCESS) {
        LOG_ERROR("ma_context_get_devices failed - %s.\n",
                  ma_result_description(getDevicesResult));

        return;
    }

    // Process playback devices
    process_device_list(context,
                        ma_device_type_playback,
                        playbackDeviceInfos,
                        playbackDeviceCount,
                        &context->playbackCache);

    // Process capture devices
    process_device_list(context,
                        ma_device_type_capture,
                        captureDeviceInfos,
                        captureDeviceCount,
                        &context->captureCache);
}

FFI_PLUGIN_EXPORT
uint32_t context_get_playback_device_count(const void *pContext) {
    if (!pContext) {
        LOG_ERROR("invalid parameter: `pContext` is NULL.\n", "");
        return -1;
    }

    context_t *context = (context_t *)pContext;

    LOG_DEBUG("playback device count: %d\n", context->playbackCache.count);

    return context->playbackCache.count;
}

FFI_PLUGIN_EXPORT
uint32_t context_get_capture_device_count(const void *pContext) {
    if (!pContext) {
        LOG_ERROR("invalid parameter: `pContext` is NULL.\n", "");
        return -1;
    }

    context_t *context = (context_t *)pContext;

    LOG_DEBUG("capture device count: %d\n", context->captureCache.count);

    return context->captureCache.count;
}

FFI_PLUGIN_EXPORT
device_info_t *context_get_playback_device_infos(const void *pContext) {
    if (!pContext) {
        LOG_ERROR("invalid parameter: `pContext` is NULL.\n", "");
        return NULL;
    }

    context_t *context = (context_t *)pContext;

    return context->playbackCache.deviceInfo;
}

FFI_PLUGIN_EXPORT
device_info_t *context_get_capture_device_infos(const void *pContext) {
    if (!pContext) {
        LOG_ERROR("invalid parameter: `pContext` is NULL.\n", "");
        return NULL;
    }

    context_t *context = (context_t *)pContext;

    return context->captureCache.deviceInfo;
}
