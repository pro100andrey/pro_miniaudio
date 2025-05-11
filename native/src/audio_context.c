#include "../include/audio_context.h"

#include <stdlib.h>
#include <string.h>

#include "../include/audio_context_private.h"
#include "../include/internal.h"
#include "../include/logger.h"
#include "../include/miniaudio.h"

void _ma_(void *pUserData, ma_uint32 level, const char *pMessage) {
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
void *audio_context_create(void) {
    audio_context_t *context = malloc(sizeof(audio_context_t));

    if (!context) {
        LOG_ERROR("failed to allocate memory for audio context.\n", "");
        return NULL;
    }

    context->audioDevices = NULL;
    context->deviceCount = 0;

    ma_context_config config = ma_context_config_init();
    config.coreaudio.sessionCategory =
        ma_ios_session_category_play_and_record;
    config.coreaudio.sessionCategoryOptions =
        ma_ios_session_category_option_allow_air_play |
        ma_ios_session_category_option_allow_bluetooth_a2dp |
            ma_ios_session_category_option_allow_bluetooth |
        ma_ios_session_category_option_default_to_speaker;

    config.coreaudio.noAudioSessionDeactivate = MA_TRUE;


    ma_log *log = malloc(sizeof(ma_log));
    if (ma_log_init(
            &context->maContext.allocationCallbacks,
            log) == MA_SUCCESS) {
        ma_log_callback log_callback;
        log_callback.onLog = _ma_;
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

    LOG_INFO("<%p>(audio_context_t *) created.\n", context);

    return context;
}

FFI_PLUGIN_EXPORT
void audio_context_destroy(void *self) {
    if (!self) {
        LOG_ERROR("invalid parameter: `self` is NULL.\n", "");

        return;
    }

    audio_context_t *ctx = (audio_context_t *)self;
    // Stop and free each device
    LOG_INFO("Destroying %d devices.\n", ctx->deviceCount);

    size_t count = ctx->deviceCount;

    for (size_t i = 0; i < count; i++) {
        audio_device_t *device = ctx->audioDevices[i];
        audio_device_vtable_t *vtable = device->vtable;

        device->vtable = NULL;

        if (vtable && vtable->destroy) {
            LOG_INFO("Destroying device <%p>(audio_device_t *)\n", device);
            vtable->destroy(device);
        }
    }

    ma_result contextUninitResult =
        ma_context_uninit(&ctx->maContext);

    if (contextUninitResult != MA_SUCCESS) {
        LOG_ERROR("ma_context_uninit failed - %s.\n",
                  ma_result_description(contextUninitResult));
    }

    free(ctx->audioDevices);
    free(ctx->maContext.pLog);
    free(ctx);

    LOG_INFO("<%p>(audio_context_t *) destroyed.\n", ctx);
}

FFI_PLUGIN_EXPORT
void audio_context_refresh_devices(const void *self) {
    if (!self) {
        LOG_ERROR("invalid parameter: `self` is NULL.\n", "");
        return;
    }

    audio_context_t *ctx = (audio_context_t *)self;

    // Get playback and capture devices
    ma_result getDevicesResult =
        ma_context_get_devices(&ctx->maContext,
                               NULL,
                               NULL,
                               NULL,
                               NULL);

    if (getDevicesResult != MA_SUCCESS) {
        LOG_ERROR("ma_context_get_devices failed - %s.\n",
                  ma_result_description(getDevicesResult));

        return;
    }

    LOG_INFO("devices refreshed.\n", "");
    LOG_DEBUG("  playback device count: %d.\n", ctx->maContext.playbackDeviceInfoCount);
    LOG_DEBUG("  capture device count: %d.\n", ctx->maContext.captureDeviceInfoCount);
}

static void _fill_device_info_list(ma_device_info *pSource,
                                   device_infos_t **ppDeviceInfos,
                                   uint32_t count);

static void _fill_device_info_ext(ma_context *maContext,
                                  ma_device_type deviceType,
                                  ma_device_id *pDeviceId,
                                  device_info_ext_t **ppDeviceInfoExt);

FFI_PLUGIN_EXPORT
device_infos_t *audio_context_get_device_infos(const void *self, audio_device_type_t type) {
    if (!self) {
        LOG_ERROR("invalid parameter: `pContext` is NULL.\n", "");
        return NULL;
    }

    audio_context_t *ctx = (audio_context_t *)self;
    uint32_t playbackCount = ctx->maContext.playbackDeviceInfoCount;
    uint32_t captureCount = ctx->maContext.captureDeviceInfoCount;

    device_infos_t *pDeviceInfos = NULL;

    switch (type) {
        case device_type_playback:

            _fill_device_info_list(ctx->maContext.pDeviceInfos,
                                   &pDeviceInfos,
                                   playbackCount);
            break;
        case device_type_capture:

            _fill_device_info_list(ctx->maContext.pDeviceInfos + playbackCount,
                                   &pDeviceInfos,
                                   captureCount);
            break;

        default: {
            LOG_ERROR("invalid device type: %d.\n", type);
            return NULL;
        }

        break;
    }

    if (!pDeviceInfos) {
        LOG_ERROR("failed to fill device info list.\n", "");
        return NULL;
    }

    pDeviceInfos->type = type;

    LOG_DEBUG("device type(%d) infos(%d) retrieved.\n", type, pDeviceInfos->count);

    return pDeviceInfos;
}

FFI_PLUGIN_EXPORT
void audio_context_device_infos_destroy(device_infos_t *pDeviceInfos) {
    if (!pDeviceInfos) {
        LOG_ERROR("invalid parameter: `pDeviceInfos` is NULL.\n", "");

        return;
    }

    audio_device_type_t type = pDeviceInfos->type;

    free(pDeviceInfos->list);
    free(pDeviceInfos);

    LOG_DEBUG("device type(%d) infos destroyed.\n", type);
}

FFI_PLUGIN_EXPORT
device_info_ext_t *audio_context_get_device_info_ext(const void *self, void *deviceId) {
    if (!self) {
        LOG_ERROR("invalid parameter: `self` is NULL.\n", "");
        return NULL;
    }

    if (!deviceId) {
        LOG_ERROR("invalid parameter: `deviceId` is NULL.\n", "");
        return NULL;
    }

    audio_context_t *ctx = (audio_context_t *)self;

    device_info_ext_t *pDeviceInfoExt = NULL;

    _fill_device_info_ext(&ctx->maContext,
                          ma_device_type_playback,
                          deviceId,
                          &pDeviceInfoExt);

    return pDeviceInfoExt;
}

FFI_PLUGIN_EXPORT
void audio_context_device_info_ext_destroy(device_info_ext_t *pDeviceInfoExt) {
    if (!pDeviceInfoExt) {
        LOG_ERROR("invalid parameter: `pDeviceInfoExt` is NULL.\n", "");
        return;
    }

    free(pDeviceInfoExt->list);
    free(pDeviceInfoExt);

    LOG_INFO("device info ext destroyed.\n", "");
}

static void _fill_device_info_list(ma_device_info *pSource,
                                   device_infos_t **ppDeviceInfos,
                                   uint32_t count) {
    if (!pSource) {
        LOG_ERROR("invalid parameter: `pSource` is NULL.\n", "");
        return;
    }

    if (count == 0) {
        LOG_WARN("no devices found.\n", "");
        return;
    }

    *ppDeviceInfos = malloc(sizeof(device_infos_t));

    if (!(*ppDeviceInfos)) {
        LOG_ERROR("failed to allocate memory for device info.\n", "");
        return;
    }

    device_info_t *list = malloc(count * sizeof(device_info_t));

    if (!list) {
        LOG_ERROR("failed to allocate memory for device info list.\n", "");
        free(ppDeviceInfos);
        *ppDeviceInfos = NULL;
        return;
    }

    for (uint32_t i = 0; i < count; i++) {
        // Copy device name
        size_t nameLength = MA_MAX_DEVICE_NAME_LENGTH + 1;
        strncpy(list[i].name, pSource[i].name, nameLength);

        // Copy device id

        memcpy(&list[i].id, &pSource[i].id, sizeof(ma_device_id));

        // Copy device default status
        list[i].isDefault = pSource[i].isDefault;
    }

    (*ppDeviceInfos)->list = list;
    (*ppDeviceInfos)->count = count;
}

static void _fill_device_info_ext(ma_context *maContext,
                                  ma_device_type deviceType,
                                  ma_device_id *pDeviceId,
                                  device_info_ext_t **ppDeviceInfoExt) {
    if (!maContext) {
        LOG_ERROR("invalid parameter: `maContext` is NULL.\n", "");
        return;
    }

    if (!pDeviceId) {
        LOG_ERROR("invalid parameter: `pDeviceId` is NULL.\n", "");
        return;
    }

    ma_device_info deviceInfo;
    ma_result getDeviceInfoResult =
        ma_context_get_device_info(maContext,
                                   deviceType,
                                   pDeviceId,
                                   &deviceInfo);

    if (getDeviceInfoResult != MA_SUCCESS) {
        LOG_ERROR("failed to get device info for %s device - %s.\n",
                  deviceType == ma_device_type_playback ? "playback" : "capture",
                  ma_result_description(getDeviceInfoResult));
        return;
    }

    *ppDeviceInfoExt = malloc(sizeof(device_info_ext_t));

    if (!(*ppDeviceInfoExt)) {
        LOG_ERROR("failed to allocate memory for device info ext.\n", "");
        return;
    }

    audio_format_t *list = malloc(deviceInfo.nativeDataFormatCount * sizeof(audio_format_t));

    if (!list) {
        LOG_ERROR("failed to allocate memory for audio format list.\n", "");
        free(ppDeviceInfoExt);
        *ppDeviceInfoExt = NULL;
        return;
    }

    for (uint32_t i = 0; i < deviceInfo.nativeDataFormatCount; i++) {
        list[i].pcmFormat = (pcm_format_t)deviceInfo.nativeDataFormats[i].format;
        list[i].channels = deviceInfo.nativeDataFormats[i].channels;
        list[i].sampleRate = deviceInfo.nativeDataFormats[i].sampleRate;
        // flag skiped
    }

    (*ppDeviceInfoExt)->list = list;
    (*ppDeviceInfoExt)->count = deviceInfo.nativeDataFormatCount;
}
