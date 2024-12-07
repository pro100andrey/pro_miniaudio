#include "../include/audio_context.h"

#include <stdlib.h>
#include <string.h>

#include "../include/logger.h"
#include "../include/miniaudio.h"

/**
 * @struct device_cache
 * @brief Structure to store device information.
 */
typedef struct device_cache {
    uint32_t deviceCount;       // Number of devices available
    device_info_t devices[64];  // Array of device information
} device_cache_t;

/**
 * @struct AudioContext
 * @brief Structure to manage the audio system context.
 */
typedef struct audio_context {
    ma_context maContext;  // Miniaudio context for audio system initialization
    bool initialized;      // Indicates if the context is initialized
    bool hasDevicesInfo;   // Indicates if device information is cached

    device_cache_t playback;  // Playback device cache
    device_cache_t capture;   // Capture device cache
} audio_context_t;

FFI_PLUGIN_EXPORT
void *audio_context_create(void) {
    audio_context_t *context =
        (audio_context_t *)malloc(sizeof(audio_context_t));

    if (!context) {
        LOG_DEBUG("Failed to allocate memory for audio context", "");
        return NULL;
    }

    ma_context_config contextConfig = ma_context_config_init();
    contextConfig.coreaudio.sessionCategory = ma_ios_session_category_play_and_record;

    contextConfig.coreaudio.sessionCategoryOptions = ma_ios_session_category_option_allow_bluetooth |
                                                     ma_ios_session_category_option_mix_with_others;

    ma_result contextInitResult =
        ma_context_init(NULL,
                        0,
                        &contextConfig,
                        &context->maContext);

    if (contextInitResult != MA_SUCCESS) {
        free(context);

        LOG_ERROR("Failed to initialize audio context - %s",
                  ma_result_description(contextInitResult));

        return NULL;
    }

    context->initialized = true;
    context->hasDevicesInfo = false;

    context->playback.deviceCount = 0;
    context->capture.deviceCount = 0;

    LOG_INFO("Audio context created <%p>.", context);

    return context;
}

FFI_PLUGIN_EXPORT
void audio_context_destroy(void *context) {
    if (!context) {
        LOG_ERROR("Invalid AudioContext provided for destruction", "");
        return;
    }

    audio_context_t *ctx = (audio_context_t *)context;

    ma_result contextUninitResult =
        ma_context_uninit(&ctx->maContext);

    if (contextUninitResult != MA_SUCCESS) {
        LOG_ERROR("Failed to uninitialize miniaudio context - %s",
                  ma_result_description(contextUninitResult));
    }

    free(ctx);

    LOG_INFO("Audio context destroyed <%p>", context);
}

FFI_PLUGIN_EXPORT
bool audio_context_is_valid(const void *context) {
    audio_context_t *audio_context = (audio_context_t *)context;

    return audio_context && audio_context->initialized;
}

static void process_device_info(const ma_device_info *nativeDeviceInfo,
                                device_info_t *deviceInfo) {
    strncpy(deviceInfo->name,
            nativeDeviceInfo->name,
            MA_MAX_DEVICE_NAME_LENGTH + 1);

    memcpy(&deviceInfo->id, &nativeDeviceInfo->id, sizeof(device_id_t));

    deviceInfo->isDefault = nativeDeviceInfo->isDefault;
    deviceInfo->dataFormatCount = nativeDeviceInfo->nativeDataFormatCount;

    for (uint32_t j = 0; j < nativeDeviceInfo->nativeDataFormatCount; j++) {
        deviceInfo->dataFormats[j].format =
            (sample_format_t)nativeDeviceInfo->nativeDataFormats[j].format;
        deviceInfo->dataFormats[j].channels =
            nativeDeviceInfo->nativeDataFormats[j].channels;
        deviceInfo->dataFormats[j].sampleRate =
            nativeDeviceInfo->nativeDataFormats[j].sampleRate;
        deviceInfo->dataFormats[j].flags =
            nativeDeviceInfo->nativeDataFormats[j].flags;
    }
}

static void process_device_list(audio_context_t *ctx,
                                ma_device_type deviceType,
                                ma_device_info *nativeDevices,
                                uint32_t deviceCount,
                                device_cache_t *deviceCache) {
    deviceCache->deviceCount = deviceCount;

    for (uint32_t i = 0; i < deviceCount; i++) {
        ma_device_info deviceInfo = {0};
        ma_result getDeviceInfoResult =
            ma_context_get_device_info(&ctx->maContext,
                                       deviceType,
                                       &nativeDevices[i].id,
                                       &deviceInfo);

        if (getDeviceInfoResult != MA_SUCCESS) {
            LOG_ERROR("Failed to get %s device info - %s",
                      deviceType == ma_device_type_playback ? "playback" : "capture",
                      ma_result_description(getDeviceInfoResult));

            continue;
        }

        process_device_info(&deviceInfo,
                            &deviceCache->devices[i]);
    }
}

FFI_PLUGIN_EXPORT
void audio_context_refresh_devices(const void *context) {
    if (!context) {
        LOG_ERROR("Invalid AudioContext provided for device refresh", "");
        return;
    }

    ma_device_info *pPlaybackInfos;
    ma_uint32 playbackCount;

    ma_device_info *pCaptureInfos;
    ma_uint32 captureCount;

    audio_context_t *ctx = (audio_context_t *)context;
    // Get playback and capture devices
    ma_result getDevicesResult =
        ma_context_get_devices(&ctx->maContext,
                               &pPlaybackInfos,
                               &playbackCount,
                               &pCaptureInfos,
                               &captureCount);

    if (getDevicesResult != MA_SUCCESS) {
        LOG_ERROR("Failed to get audio devices - %s",
                  ma_result_description(getDevicesResult));

        return;
    }

    // Process playback devices
    process_device_list(ctx,
                        ma_device_type_playback,
                        pPlaybackInfos,
                        playbackCount,
                        &ctx->playback);

    // Process capture devices
    process_device_list(ctx,
                        ma_device_type_capture,
                        pCaptureInfos,
                        captureCount,
                        &ctx->capture);
}

FFI_PLUGIN_EXPORT
uint32_t audio_context_get_playback_device_count(const void *context) {
    if (!context) {
        LOG_ERROR("Invalid AudioContext provided for playback device count", "");
        return -1;
    }

    audio_context_t *audio_context = (audio_context_t *)context;

    return audio_context->playback.deviceCount;
}

FFI_PLUGIN_EXPORT
uint32_t audio_context_get_capture_device_count(const void *context) {
    if (!context) {
        LOG_ERROR("Invalid AudioContext provided for capture device count", "");
        return -1;
    }

    audio_context_t *audio_context = (audio_context_t *)context;

    return audio_context->capture.deviceCount;
}

FFI_PLUGIN_EXPORT
device_info_t *audio_context_get_playback_devices_info(const void *context) {
    if (!context) {
        LOG_ERROR("Invalid AudioContext provided for playback devices info", "");
        return NULL;
    }

    audio_context_t *audio_context = (audio_context_t *)context;

    return audio_context->playback.devices;
}

FFI_PLUGIN_EXPORT
device_info_t *audio_context_get_capture_devices_info(const void *context) {
    if (!context) {
        LOG_ERROR("Invalid AudioContext provided for capture devices info", "");
        return NULL;
    }

    audio_context_t *audio_context = (audio_context_t *)context;

    return audio_context->capture.devices;
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
