#include "../include/playback_device.h"

#include <stdlib.h>
#include <string.h>

#include "../include/logger.h"
#include "../include/miniaudio.h"

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
        LOG_ERROR("Invalid device context", "");
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

    LOG_INFO("Playback device created <%p>.", deviceCtx);
    LOG_INFO("Playback buffer created size: %zu (bytes) <%p>.",
             bufferSizeInBytes, &deviceCtx->pcmRingBuffer);

    return result_ptr(deviceCtx);
}

FFI_PLUGIN_EXPORT
void playback_device_destroy(void *device) {
    if (!device) {
        LOG_ERROR("Invalid playback device provided for destruction", "");
        return;
    }

    device_context_t *deviceCtx = (device_context_t *)device;

    if (ma_device_is_started(&deviceCtx->device)) {
        ma_device_stop(&deviceCtx->device);
    }

    ma_device_uninit(&deviceCtx->device);
    ma_rb_uninit(&deviceCtx->pcmRingBuffer);

    LOG_INFO("Playback buffer destroyed <%p>.", &deviceCtx->pcmRingBuffer);

    free(deviceCtx);

    LOG_INFO("Playback device destroyed <%p>.", device);
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
