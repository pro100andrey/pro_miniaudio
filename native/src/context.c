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
    ma_context maContext;  // Miniaudio context for audio system initialization
    bool initialized;      // Indicates if the context is initialized

    devices_cache_t playbackCache;  // Playback device cache
    devices_cache_t captureCache;   // Capture device cache
} context_t;

FFI_PLUGIN_EXPORT
void *context_create(void) {
    context_t *pContext = (context_t *)malloc(sizeof(context_t));

    if (!pContext) {
        LOG_DEBUG("failed to allocate memory for context_t", "");
        return NULL;
    }

    ma_context_config contextConfig = ma_context_config_init();
    contextConfig.coreaudio.sessionCategory =
        ma_ios_session_category_play_and_record;

    contextConfig.coreaudio.sessionCategoryOptions =
        ma_ios_session_category_option_allow_bluetooth |
        ma_ios_session_category_option_mix_with_others;

    ma_result maContextR = ma_context_init(NULL,
                                           0,
                                           &contextConfig,
                                           &pContext->maContext);

    if (maContextR != MA_SUCCESS) {
        free(pContext);

        LOG_ERROR("failed to initialize ma_context - %s",
                  ma_result_description(maContextR));

        return NULL;
    }

    pContext->initialized = true;
    pContext->playbackCache.count = 0;
    pContext->captureCache.count = 0;

    LOG_INFO("context_t created <%p>.", pContext);

    return pContext;
}

FFI_PLUGIN_EXPORT
void context_destroy(void *p) {
    if (!p) {
        LOG_ERROR("invalid context_t provided for destruction", "");
        return;
    }

    context_t *pContext = (context_t *)p;

    ma_result maContextUninitResult =
        ma_context_uninit(&pContext->maContext);

    if (maContextUninitResult != MA_SUCCESS) {
        LOG_ERROR("failed to uninitialize ma_context - %s",
                  ma_result_description(maContextUninitResult));
    }

    if (pContext->playbackCache.deviceInfo) {
        free(pContext->playbackCache.deviceInfo);
    }

    if (pContext->captureCache.deviceInfo) {
        free(pContext->captureCache.deviceInfo);
    }

    free(pContext);

    LOG_INFO("context_t destroyed <%p>", pContext);
}

FFI_PLUGIN_EXPORT
bool context_is_valid(const void *p) {
    context_t *pContext = (context_t *)p;

    return pContext && pContext->initialized;
}

static void process_device_info(const ma_device_info *maDeviceInfo,
                                device_info_t *deviceInfo) {
    strncpy(deviceInfo->name,
            maDeviceInfo->name,
            MA_MAX_DEVICE_NAME_LENGTH + 1);

    memcpy(&deviceInfo->id, &maDeviceInfo->id, sizeof(device_id_t));

    deviceInfo->isDefault = maDeviceInfo->isDefault;
    deviceInfo->dataFormatCount = maDeviceInfo->nativeDataFormatCount;

    for (uint32_t j = 0; j < maDeviceInfo->nativeDataFormatCount; j++) {
        deviceInfo->dataFormats[j].format =
            (sample_format_t)maDeviceInfo->nativeDataFormats[j].format;
        deviceInfo->dataFormats[j].channels =
            maDeviceInfo->nativeDataFormats[j].channels;
        deviceInfo->dataFormats[j].sampleRate =
            maDeviceInfo->nativeDataFormats[j].sampleRate;
        deviceInfo->dataFormats[j].flags =
            maDeviceInfo->nativeDataFormats[j].flags;
    }
}

static void process_device_list(context_t *context,
                                ma_device_type deviceType,
                                ma_device_info *deviceInfos,
                                uint32_t deviceCount,
                                devices_cache_t *devicesCache) {
    //
    devicesCache->count = deviceCount;
    devicesCache->deviceInfo =
        (device_info_t *)malloc(deviceCount * sizeof(device_info_t));

    for (uint32_t i = 0; i < deviceCount; i++) {
        ma_device_info deviceInfo = {0};
        ma_device_id *deviceId = &deviceInfos[i].id;
        ma_result getDeviceInfoResult =
            ma_context_get_device_info(&context->maContext,
                                       deviceType,
                                       deviceId,
                                       &deviceInfo);

        if (getDeviceInfoResult != MA_SUCCESS) {
            LOG_ERROR("failed to get %s device info - %s",
                      deviceType == ma_device_type_playback ? "playback" : "capture",
                      ma_result_description(getDeviceInfoResult));

            continue;
        }

        process_device_info(&deviceInfo,
                            &devicesCache->deviceInfo[i]);
    }
}

FFI_PLUGIN_EXPORT
void context_refresh_devices(const void *context) {
    if (!context) {
        LOG_ERROR("invalid context_t provided for device refresh", "");
        return;
    }

    ma_device_info *playbackDeviceInfos;
    ma_uint32 playbackDeviceCount;

    ma_device_info *captureDeviceInfos;
    ma_uint32 captureDeviceCount;

    context_t *ctx = (context_t *)context;
    ma_context *maContext = &ctx->maContext;
    // Get playback and capture devices
    ma_result getDevicesResult =
        ma_context_get_devices(maContext,
                               &playbackDeviceInfos,
                               &playbackDeviceCount,
                               &captureDeviceInfos,
                               &captureDeviceCount);

    if (getDevicesResult != MA_SUCCESS) {
        LOG_ERROR("failed to get audio devices - %s",
                  ma_result_description(getDevicesResult));

        return;
    }

    // Free previous devices
    if (ctx->playbackCache.deviceInfo) {
        ctx->playbackCache.count = 0;
        free(ctx->playbackCache.deviceInfo);
    }
    // Free previous devices
    if (ctx->captureCache.deviceInfo) {
        ctx->captureCache.count = 0;
        free(ctx->captureCache.deviceInfo);
    }

    // Process playback devices
    process_device_list(ctx,
                        ma_device_type_playback,
                        playbackDeviceInfos,
                        playbackDeviceCount,
                        &ctx->playbackCache);

    // Process capture devices
    process_device_list(ctx,
                        ma_device_type_capture,
                        captureDeviceInfos,
                        captureDeviceCount,
                        &ctx->captureCache);
}

FFI_PLUGIN_EXPORT
uint32_t context_get_playback_device_count(const void *p) {
    if (!p) {
        LOG_ERROR("invalid context_t provided for playback device count", "");
        return -1;
    }

    context_t *pContext = (context_t *)p;

    return pContext->playbackCache.count;
}

FFI_PLUGIN_EXPORT
uint32_t context_get_capture_device_count(const void *p) {
    if (!p) {
        LOG_ERROR("invalid context_t provided for capture device count", "");
        return -1;
    }

    context_t *pContext = (context_t *)p;

    return pContext->captureCache.count;
}

FFI_PLUGIN_EXPORT
device_info_t *context_get_playback_device_infos(const void *p) {
    if (!p) {
        LOG_ERROR("invalid context_t provided for playback devices info", "");
        return NULL;
    }

    context_t *pContext = (context_t *)p;

    return pContext->playbackCache.deviceInfo;
}

FFI_PLUGIN_EXPORT
device_info_t *context_get_capture_device_infos(const void *p) {
    if (!p) {
        LOG_ERROR("invalid context_t provided for capture devices info", "");
        return NULL;
    }

    context_t *pContext = (context_t *)p;

    return pContext->captureCache.deviceInfo;
}

FFI_PLUGIN_EXPORT
uint32_t get_bytes_per_sample(sample_format_t format) {
    ma_format maFormat = (ma_format)format;

    return ma_get_bytes_per_sample(maFormat);
}

FFI_PLUGIN_EXPORT
uint32_t get_bytes_per_frame(sample_format_t format, uint32_t channels) {
    ma_format maFormat = (ma_format)format;

    return ma_get_bytes_per_frame(maFormat, channels);
}
