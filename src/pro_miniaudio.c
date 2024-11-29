#include "pro_miniaudio.h"

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "miniaudio/miniaudio.h"
#include "resource_manager.h"

FFI_PLUGIN_EXPORT
void resource_manager_free_all(void) {
    resource_manager_clear();
}

// Define cleanup functions for the resource manager using a macro.
// The macro generates a unique cleanup function for each provided function,
// reducing repetitive boilerplate code and ensuring consistency.
//
// Each generated cleanup function has the following structure:
// - Accepts a `void *` pointer as an argument (the resource to be cleaned up).
// - Calls the specified cleanup function with the provided pointer.
//
// This approach simplifies the registration of resources with the resource manager
// and ensures that resources are cleaned up properly when required.
//
// Usage example:
// GENERATE_CLEANUP_FUNC(audio_context_destroy);
// This expands to:
// static void audio_context_destroy_cleanup(void *context) {
//     audio_context_destroy(context);
// }
//
// Below are the cleanup functions generated for specific resources:
GENERATE_CLEANUP_FUNC(audio_context_destroy)
GENERATE_CLEANUP_FUNC(playback_device_destroy)
GENERATE_CLEANUP_FUNC(waveform_destroy)

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

static result_t result_from_int(int32_t value) {
    return (result_t){
        .data.intData = value,
        .type = data_type_int,
    };
}

static result_t result_from_ptr(void *pointer) {
    return (result_t){
        .data.pData = pointer,
        .type = data_type_ptr,
    };
}

static result_t result_error(error_code_t errorCode, const char *format, ...) {
    va_list args;
    va_start(args, format);

    result_t result = {
        .type = data_type_empty,
        .code = errorCode,
    };

    vsnprintf(result.message, sizeof(result.message) - 1, format, args);
    result.message[sizeof(result.message) - 1] = '\0';

    va_end(args);

    LOG_ERROR(result.message, );

    return result;
}

FFI_PLUGIN_EXPORT
result_t audio_context_create(void) {
    audio_context_t *context =
        (audio_context_t *)malloc(sizeof(audio_context_t));

    if (!context) {
        return result_error(
            error_code_context,
            "Failed to allocate memory for AudioContext");
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

        return result_error(error_code_context,
                            "Failed to initialize audio context - %s",
                            ma_result_description(contextInitResult));
    }

    context->initialized = true;
    context->hasDevicesInfo = false;

    context->playback.deviceCount = 0;
    context->capture.deviceCount = 0;

    if (!resource_manager_register(context, audio_context_destroy_cleanup)) {
        audio_context_destroy(context);

        return result_error(error_code_context,
                            "Failed to register AudioContext");
    }

    LOG_INFO("Audio context created <%p>.", context);

    return result_from_ptr(context);
}

FFI_PLUGIN_EXPORT
result_t audio_context_destroy(void *context) {
    if (!context) {
        return result_error(error_code_context,
                            "Invalid audio context");
    }

    audio_context_t *ctx = (audio_context_t *)context;

    resource_manager_unregister(ctx);

    if (ctx->initialized) {
        ma_result contextUninitResult =
            ma_context_uninit(&ctx->maContext);

        if (contextUninitResult != MA_SUCCESS) {
            return result_error(error_code_context,
                                "Failed to uninitialize audio context - %s",
                                ma_result_description(contextUninitResult));
        }
    }

    free(ctx);

    LOG_INFO("Audio context destroyed <%p>", context);

    return (result_t){0};
}

FFI_PLUGIN_EXPORT
bool audio_context_is_valid(const void *context) {
    audio_context_t *audio_context = (audio_context_t *)context;

    return audio_context && audio_context->initialized;
}

#define RETURN_IF_AUDIO_CONTEXT_INVALID(context) \
    do {                                         \
        if (!audio_context_is_valid(context)) {  \
            return result_error(                 \
                error_code_context,              \
                "Invalid AudioContext");         \
        }                                        \
    } while (0)

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

static result_t process_device_list(audio_context_t *ctx,
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
            return result_error(error_code_get_device_info,
                                "Failed to get %s device info - %s",
                                deviceType == ma_device_type_playback ? "playback" : "capture",
                                ma_result_description(getDeviceInfoResult));
        }

        process_device_info(&deviceInfo,
                            &deviceCache->devices[i]);
    }

    return (result_t){0};
}

FFI_PLUGIN_EXPORT
result_t audio_context_refresh_devices(const void *context) {
    RETURN_IF_AUDIO_CONTEXT_INVALID(context);

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
        return result_error(error_code_get_device,
                            "Failed to get audio devices - %s",
                            ma_result_description(getDevicesResult));
    }

    // Process playback devices
    result_t playbackResult =
        process_device_list(ctx,
                            ma_device_type_playback,
                            pPlaybackInfos,
                            playbackCount,
                            &ctx->playback);

    if (playbackResult.code != error_code_none) {
        return playbackResult;
    }

    // Process capture devices
    result_t captureResult =
        process_device_list(ctx,
                            ma_device_type_capture,
                            pCaptureInfos,
                            captureCount,
                            &ctx->capture);

    return captureResult;
}

FFI_PLUGIN_EXPORT
result_t audio_context_get_playback_device_count(const void *context) {
    RETURN_IF_AUDIO_CONTEXT_INVALID(context);
    audio_context_t *audio_context = (audio_context_t *)context;

    return result_from_int(audio_context->playback.deviceCount);
}

FFI_PLUGIN_EXPORT
result_t audio_context_get_capture_device_count(const void *context) {
    RETURN_IF_AUDIO_CONTEXT_INVALID(context);

    audio_context_t *audio_context = (audio_context_t *)context;

    return result_from_int(audio_context->capture.deviceCount);
}

FFI_PLUGIN_EXPORT
result_t audio_context_get_playback_devices_info(const void *context) {
    RETURN_IF_AUDIO_CONTEXT_INVALID(context);

    audio_context_t *audio_context = (audio_context_t *)context;

    return result_from_ptr(audio_context->playback.devices);
}

FFI_PLUGIN_EXPORT
result_t audio_context_get_capture_devices_info(const void *context) {
    RETURN_IF_AUDIO_CONTEXT_INVALID(context);

    audio_context_t *audio_context = (audio_context_t *)context;

    return result_from_ptr(audio_context->capture.devices);
}

////////////////////////////////////////////////////////////////////////////////

typedef struct device_context {
    ma_device device;     // Playback device
    ma_rb pcmRingBuffer;  // PCM ring buffer
} device_context_t;

void data_callback(ma_device *pDevice,
                   void *pOutput,
                   const void *pInput,
                   ma_uint32 frameCount) {
    (void)pInput;

    device_context_t *ctx = (device_context_t *)pDevice->pUserData;

    if (!ctx) {
        assert(ctx != NULL);
        return;
    }

    ma_uint32 bpf =
        ma_get_bytes_per_frame(pDevice->playback.format,
                               pDevice->playback.channels);

    ma_uint32 availableRead =
        ma_rb_available_read(&ctx->pcmRingBuffer);

    if (availableRead == 0) {
        return;
    }

    ma_uint32 framesInBytes = frameCount * bpf;

    // Buffering
    if (availableRead < framesInBytes) {
        return;
    }

    void *pReadBuffer;
    size_t pSizeInBytes = frameCount * bpf;
    // Acquire read buffer
    ma_result readResult =
        ma_rb_acquire_read(&ctx->pcmRingBuffer,
                           &pSizeInBytes,
                           &pReadBuffer);

    if (readResult != MA_SUCCESS) {
        LOG_ERROR("Failed to acquire read buffer. %s",
                  ma_result_description(readResult));
        return;
    }

    memcpy(pOutput, pReadBuffer, pSizeInBytes);

    ma_result commitResult =
        ma_rb_commit_read(&ctx->pcmRingBuffer,
                          pSizeInBytes);

    if (commitResult != MA_SUCCESS) {
        LOG_ERROR("Failed to commit read buffer. %s",
                  ma_result_description(commitResult));
    }
}

void notification_callback(const ma_device_notification *pNotification) {
    switch (pNotification->type) {
        case ma_device_notification_type_started:
            LOG_INFO("Device started <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_stopped:
            LOG_INFO("Device stopped <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_rerouted:
            LOG_INFO("Device rerouted <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_interruption_began:
            LOG_INFO("Device interruption began <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_interruption_ended:
            LOG_INFO("Device interruption ended <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_unlocked:
            LOG_INFO("Device unlocked <%p>.", pNotification->pDevice);
            break;
        default:
            break;
    }
}

FFI_PLUGIN_EXPORT
result_t playback_device_create(void *pAudioContext,
                                size_t bufferSizeInBytes,
                                device_id_t deviceId,
                                supported_format_t supportedFormat) {
    ma_context *audioCtx = (ma_context *)pAudioContext;

    if (!audioCtx) {
        return result_error(error_code_context,
                            "Invalid AudioContext");
    }

    device_context_t *deviceCtx = malloc(sizeof(device_context_t));

    if (!deviceCtx) {
        return result_error(error_code_device,
                            "Failed to allocate memory for device context");
    }

    ma_device_id pDeviceId;
    memcpy(&pDeviceId, &deviceId, sizeof(deviceId));

    uint32_t channels = supportedFormat.channels;
    uint32_t sampleRate = supportedFormat.sampleRate;
    ma_format format = (ma_format)supportedFormat.format;

    // Initialize the playback device
    ma_device_config config =
        ma_device_config_init(ma_device_type_playback);

    config.playback.format = format;
    config.playback.channels = channels;
    config.playback.pDeviceID = &pDeviceId;
    config.sampleRate = sampleRate;
    config.dataCallback = data_callback;
    config.notificationCallback = notification_callback;
    config.pUserData = deviceCtx;

    ma_result deviceInitResult = ma_device_init(audioCtx,
                                                &config,
                                                &deviceCtx->device);

    if (deviceInitResult != MA_SUCCESS) {
        free(deviceCtx);

        return result_error(error_code_device,
                            "Failed to initialize playback device - %s",
                            ma_result_description(deviceInitResult));
    }

    // Initialize the ring buffer
    ma_result rbInitResult =
        ma_rb_init(bufferSizeInBytes,
                   NULL,
                   NULL,
                   &deviceCtx->pcmRingBuffer);

    if (rbInitResult != MA_SUCCESS) {
        free(deviceCtx);

        return result_error(error_code_buffer,
                            "Failed to initialize PCM ring buffer - %s",
                            ma_result_description(rbInitResult));
    }

    if (!resource_manager_register(deviceCtx, playback_device_destroy_cleanup)) {
        playback_device_destroy(deviceCtx);

        return result_error(error_code_device,
                            "Failed to register playback device");
    }

    LOG_INFO("Playback device created <%p>.", deviceCtx);
    LOG_INFO("Playback buffer created size: %zu (bytes) <%p>.",
             bufferSizeInBytes, &deviceCtx->pcmRingBuffer);

    return result_from_ptr(deviceCtx);
}

FFI_PLUGIN_EXPORT
result_t playback_device_destroy(void *device) {
    if (!device) {
        return result_error(error_code_device,
                            "Invalid playback device");
    }

    device_context_t *deviceCtx = (device_context_t *)device;

    resource_manager_unregister(deviceCtx);

    if (ma_device_is_started(&deviceCtx->device)) {
        ma_device_stop(&deviceCtx->device);
    }

    ma_device_uninit(&deviceCtx->device);
    ma_rb_uninit(&deviceCtx->pcmRingBuffer);

    LOG_INFO("Playback buffer destroyed <%p>.", &deviceCtx->pcmRingBuffer);

    free(deviceCtx);

    LOG_INFO("Playback device destroyed <%p>.", device);

    return (result_t){0};
}

FFI_PLUGIN_EXPORT
result_t playback_device_start(void *device) {
    device_context_t *ctx = (device_context_t *)device;

    if (ctx == NULL) {
        return result_error(error_code_device,
                            "Invalid playback device");
    }

    if (ctx->device.type != ma_device_type_playback) {
        return result_error(error_code_device,
                            "Invalid device type");
    }

    if (ma_device_is_started(&ctx->device)) {
        return result_error(error_code_device,
                            "Playback device is already started");
    }

    if (ma_device_start(&ctx->device) != MA_SUCCESS) {
        return result_error(error_code_device,
                            "Failed to start playback device");
    }

    return (result_t){0};
}

FFI_PLUGIN_EXPORT
result_t playback_device_stop(void *device) {
    device_context_t *ctx = (device_context_t *)device;

    ma_result stopResult = ma_device_stop(&ctx->device);
    if (stopResult != MA_SUCCESS) {
        return result_error(error_code_device,
                            "Failed to stop playback device");
    }

    return (result_t){0};
}

FFI_PLUGIN_EXPORT
result_t playback_device_push_buffer(void *device, playback_data_t *data) {
    if (device == NULL || data == NULL || data->pUserData == NULL) {
        return result_error(error_code_device,
                            "Invalid parameters");
    }

    device_context_t *ctx = (device_context_t *)device;
    ma_format format = ctx->device.playback.format;

    if (format != (ma_format)data->format) {
        return result_error(
            error_code_device,
            "Invalid format");
    }

    ma_uint32 availableWrite = ma_rb_available_write(&ctx->pcmRingBuffer);
    ma_uint32 availableRead = ma_rb_available_read(&ctx->pcmRingBuffer);
    size_t pSizeInBytes = data->sizeInBytes;

    if (availableWrite < pSizeInBytes) {
        LOG_INFO("Buffer is full. Dropping frames", );

        size_t framesToRemove = pSizeInBytes - availableWrite;

        if (framesToRemove > availableRead) {
            framesToRemove = availableRead;
        }

        void *ppBufferOut;
        ma_result readResult =
            ma_rb_acquire_read(&ctx->pcmRingBuffer,
                               &framesToRemove,
                               &ppBufferOut);

        if (readResult != MA_SUCCESS) {
            return result_error(error_code_buffer,
                                "Failed to acquire read buffer - %s",
                                ma_result_description(readResult));
        }

        ma_result comitResult =
            ma_rb_commit_read(&ctx->pcmRingBuffer,
                              framesToRemove);

        if (comitResult != MA_SUCCESS) {
            return result_error(error_code_buffer,
                                "Failed to commit read buffer - %s",
                                ma_result_description(comitResult));
        }
    }

    // Acquire write buffer
    void *ppBufferOut;

    ma_result writeResult =
        ma_rb_acquire_write(&ctx->pcmRingBuffer,
                            &pSizeInBytes,
                            &ppBufferOut);

    if (writeResult != MA_SUCCESS) {
        return result_error(
            error_code_buffer,
            "Failed to acquire write buffer - %s",
            ma_result_description(writeResult));
    }

    memcpy(ppBufferOut, data->pUserData, pSizeInBytes);

    ma_result commitResult = ma_rb_commit_write(
        &ctx->pcmRingBuffer,
        pSizeInBytes);

    if (commitResult != MA_SUCCESS) {
        return result_error(
            error_code_buffer,
            "Failed to commit write buffer - %s",
            ma_result_description(commitResult));
    }

    return (result_t){0};
}

////////////////////////////////////////////////////////////////////////////////
FFI_PLUGIN_EXPORT
result_t waveform_create(sample_format_t format,
                         u_int32_t channels,
                         uint32_t sampleRate,
                         waveform_type_t waveformType,
                         double amplitude,
                         double frequency) {
    ma_waveform_type type = (ma_waveform_type)waveformType;

    ma_waveform_config waveformConfig = ma_waveform_config_init(
        (ma_format)format,
        channels,
        sampleRate,
        type,
        amplitude,
        frequency);

    ma_waveform *waveform = (ma_waveform *)malloc(sizeof(ma_waveform));
    if (waveform == NULL) {
        return result_error(
            error_waveform,
            "Failed to allocate memory for waveform");
    }

    ma_result waveformInitResult = ma_waveform_init(&waveformConfig,
                                                    waveform);

    if (waveformInitResult != MA_SUCCESS) {
        free(waveform);

        return result_error(
            error_waveform,
            "Failed to create waveform - %s",
            ma_result_description(waveformInitResult));
    }

    if (!resource_manager_register(waveform, waveform_destroy_cleanup)) {
        waveform_destroy(waveform);

        return result_error(
            error_waveform,
            "Failed to register waveform");
    }

    LOG_INFO("Waveform created <%p>.", waveform);

    return result_from_ptr(waveform);
}

FFI_PLUGIN_EXPORT
void waveform_destroy(void *waveform) {
    ma_waveform *pWaveform = (ma_waveform *)waveform;
    ma_waveform_uninit(pWaveform);
    free(pWaveform);

    LOG_INFO("Waveform destroyed <%p>.", waveform);
}

FFI_PLUGIN_EXPORT
result_t waveform_read_pcm_frames_with_buffer(void *waveform,
                                              void *pFramesOut,
                                              uint64_t framesCount,
                                              uint64_t *pFramesRead) {
    if (waveform == NULL || pFramesOut == NULL) {
        return result_error(error_waveform,
                            "Invalid parameters");
    }

    ma_waveform *pWaveform = (ma_waveform *)waveform;

    ma_result readResult =
        ma_waveform_read_pcm_frames(pWaveform,
                                    pFramesOut,
                                    framesCount,
                                    pFramesRead);

    if (readResult != MA_SUCCESS) {
        return result_error(
            error_waveform,
            "Failed to read waveform PCM frames - %s",
            ma_result_description(readResult));
    }

    return (result_t){0};
}

////////////////////////////////////////////////////////////////////////////////

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
