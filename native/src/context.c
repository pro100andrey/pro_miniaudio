#include "../include/context.h"

#include <stdlib.h>
#include <string.h>

#include "../include/logger.h"
#include "../include/miniaudio.h"

/**
 * @struct devices_cache_t
 * @brief Structure to store device information.
 */
typedef struct {
    uint32_t count;
    device_info_t *deviceInfo;
} devices_cache_t;

/**
 * @struct context_t
 * @brief Structure to manage the audio system context.
 */
typedef struct {
    ma_context maContext;           // Miniaudio context for audio system initialization
    devices_cache_t playbackCache;  // Playback device cache
    devices_cache_t captureCache;   // Capture device cache
} context_t;

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
    context_t *context = (context_t *)malloc(sizeof(context_t));

    if (!context) {
        LOG_ERROR("failed to allocate memory for context.\n", "");
        return NULL;
    }

    ma_context_config config = ma_context_config_init();
    config.coreaudio.sessionCategory =
        ma_ios_session_category_play_and_record;
    config.coreaudio.sessionCategoryOptions =
        ma_ios_session_category_option_allow_bluetooth |
        ma_ios_session_category_option_allow_bluetooth_a2dp |
        ma_ios_session_category_option_allow_air_play |
        ma_ios_session_category_option_mix_with_others;

    ma_log log;
    if (ma_log_init(
            &context->maContext.allocationCallbacks,
            &log) == MA_SUCCESS) {
        ma_log_callback log_callback;
        log_callback.onLog = _onLog;
        ma_log_register_callback(&log, log_callback);
        config.pLog = &log;
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

    free(context);

    LOG_INFO("<%p>(context_t *) destroyed.\n", context);
}

static void process_device_info(const ma_device_info *maDeviceInfo,
                                device_info_t *deviceInfo) {
    strncpy(deviceInfo->name,
            maDeviceInfo->name,
            MA_MAX_DEVICE_NAME_LENGTH + 1);

    memcpy(&deviceInfo->id, &maDeviceInfo->id, sizeof(device_id_t));

    deviceInfo->isDefault = maDeviceInfo->isDefault;
    deviceInfo->formatCount = maDeviceInfo->nativeDataFormatCount;

    for (uint32_t j = 0; j < maDeviceInfo->nativeDataFormatCount; j++) {
        deviceInfo->audioFormats[j].sampleFormat =
            (sample_format_t)maDeviceInfo->nativeDataFormats[j].format;
        deviceInfo->audioFormats[j].channels =
            maDeviceInfo->nativeDataFormats[j].channels;
        deviceInfo->audioFormats[j].sampleRate =
            maDeviceInfo->nativeDataFormats[j].sampleRate;
        deviceInfo->audioFormats[j].flags =
            maDeviceInfo->nativeDataFormats[j].flags;
    }
}

static void process_device_list(context_t *pContext,
                                ma_device_type deviceType,
                                ma_device_info *pMaDeviceInfos,
                                uint32_t deviceCount,
                                devices_cache_t *pDevicesCache) {
    //
    pDevicesCache->count = deviceCount;
    pDevicesCache->deviceInfo =
        (device_info_t *)malloc(deviceCount * sizeof(device_info_t));

    for (uint32_t i = 0; i < deviceCount; i++) {
        ma_device_info deviceInfo = {0};
        ma_device_id *deviceId = &pMaDeviceInfos[i].id;
        ma_result getDeviceInfoResult =
            ma_context_get_device_info(&pContext->maContext,
                                       deviceType,
                                       deviceId,
                                       &deviceInfo);

        if (getDeviceInfoResult != MA_SUCCESS) {
            LOG_ERROR("failed to get device info for %s device - %s.\n",
                      deviceType == ma_device_type_playback ? "playback" : "capture",
                      ma_result_description(getDeviceInfoResult));

            continue;
        }

        process_device_info(&deviceInfo,
                            &pDevicesCache->deviceInfo[i]);
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

    // Free previous devices
    if (context->playbackCache.deviceInfo) {
        context->playbackCache.count = 0;
        free(context->playbackCache.deviceInfo);
    }
    // Free previous devices
    if (context->captureCache.deviceInfo) {
        context->captureCache.count = 0;
        free(context->captureCache.deviceInfo);
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
