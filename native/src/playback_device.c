#include "../include/playback_device.h"

#include <stdlib.h>
#include <string.h>

#include "../include/logger.h"
#include "../include/miniaudio.h"

typedef struct {
    ma_device device;  // Playback device
    ma_rb rb;          // Ring buffer
} playback_device_t;

void data_callback(ma_device *pDevice,
                   void *pOutput,
                   const void *pInput,
                   ma_uint32 frameCount) {
    (void)pInput;

    playback_device_t *playbackDevice =
        (playback_device_t *)pDevice->pUserData;

    if (!playbackDevice) {
        LOG_ERROR("invalid playback_device_t provided for data callback", "");
        return;
    }

    ma_uint32 bpf = ma_get_bytes_per_frame(pDevice->playback.format,
                                           pDevice->playback.channels);
    ma_uint32 availableRead =
        ma_rb_available_read(&playbackDevice->rb);

    if (availableRead == 0) {
        LOG_INFO("No data available for playback", "");

        return;
    }

    ma_uint32 bytesPerFrames = frameCount * bpf;
    size_t bytesToRead = (availableRead < bytesPerFrames) ? availableRead : bytesPerFrames;
    size_t totalProcessed = 0;

    while (bytesToRead > 0) {
        void *bufferOut;
        size_t chunkSize = bytesToRead;

        ma_result readResult = ma_rb_acquire_read(&playbackDevice->rb,
                                                  &chunkSize,
                                                  &bufferOut);
        if (readResult != MA_SUCCESS) {
            LOG_ERROR("Failed to acquire read buffer: %s",
                      ma_result_description(readResult));
            break;
        }

        if (chunkSize == 0) {
            LOG_ERROR("No more data available for reading.");
            break;
        }

        // Обработка данных из текущего сегмента
        memcpy(pOutput, bufferOut, chunkSize);
        pOutput = (char *)pOutput + chunkSize;
        totalProcessed += chunkSize;
        bytesToRead -= chunkSize;

        ma_result commitResult = ma_rb_commit_read(
            &playbackDevice->rb,
            chunkSize);

        if (commitResult == MA_AT_END) {
            LOG_INFO("Reached the end of available data in the ring buffer.");
            break;
        } else if (commitResult != MA_SUCCESS) {
            LOG_ERROR("Failed to commit read buffer: %s",
                      ma_result_description(commitResult));
            break;
        }
    }
}

void notification_callback(const ma_device_notification *pNotification) {
    switch (pNotification->type) {
        case ma_device_notification_type_started:
            LOG_INFO("device started <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_stopped:
            LOG_INFO("device stopped <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_rerouted:
            LOG_INFO("device rerouted <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_interruption_began:
            LOG_INFO("device interruption began <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_interruption_ended:
            LOG_INFO("device interruption ended <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_unlocked:
            LOG_INFO("device unlocked <%p>.", pNotification->pDevice);
            break;
        default:
            break;
    }
}

FFI_PLUGIN_EXPORT
void *playback_device_create(const void *pMaContext,
                             size_t bufferSizeInBytes,
                             device_id_t deviceId,
                             supported_format_t supportedFormat) {
    if (!pMaContext) {
        LOG_ERROR("Invalid context_t provided", "");

        return NULL;
    }

    if (bufferSizeInBytes == 0) {
        LOG_ERROR("Invalid buffer size", "");

        return NULL;
    }

    ma_context *pContext = (ma_context *)pMaContext;
    playback_device_t *pPlaybackDevice = malloc(sizeof(playback_device_t));

    if (!pPlaybackDevice) {
        LOG_ERROR("Failed to allocate memory for playback_device_t", "");

        return NULL;
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
    config.pUserData = pPlaybackDevice;

    ma_result maDeviceInitR = ma_device_init(pContext,
                                             &config,
                                             &pPlaybackDevice->device);

    if (maDeviceInitR != MA_SUCCESS) {
        free(pPlaybackDevice);

        LOG_ERROR("failed to initialize playback ma_device - %s",
                  ma_result_description(maDeviceInitR));

        return NULL;
    }

    // Initialize the ring buffer
    ma_result maRBInitResult =
        ma_rb_init(bufferSizeInBytes,
                   NULL,
                   NULL,
                   &pPlaybackDevice->rb);

    if (maRBInitResult != MA_SUCCESS) {
        free(pPlaybackDevice);

        LOG_ERROR("failed to initialize ma_rb - %s",
                  ma_result_description(maRBInitResult));

        return NULL;
    }

    LOG_INFO("Playback device created <%p>.", pPlaybackDevice);
    LOG_INFO("Playback buffer created size: %zu (bytes) <%p>.",
             bufferSizeInBytes, &pPlaybackDevice->rb);

    return pPlaybackDevice;
}

FFI_PLUGIN_EXPORT
void playback_device_destroy(void *pDevice) {
    if (!pDevice) {
        LOG_ERROR("invalid playback device provided for destruction", "");
        return;
    }

    playback_device_t *deviceCtx =
        (playback_device_t *)pDevice;

    if (ma_device_is_started(&deviceCtx->device)) {
        ma_device_stop(&deviceCtx->device);
    }

    ma_device_uninit(&deviceCtx->device);
    ma_rb_uninit(&deviceCtx->rb);

    LOG_INFO("Playback buffer destroyed <%p>.", &deviceCtx->rb);

    free(deviceCtx);

    LOG_INFO("Playback device destroyed <%p>.", pDevice);
}

FFI_PLUGIN_EXPORT void playback_device_start(void *pPlaybackDevice) {
    if (pPlaybackDevice == NULL) {
        LOG_ERROR("Invalid playback device", "");

        return;
    }

    playback_device_t *ctx = (playback_device_t *)pPlaybackDevice;

    if (ctx->device.type != ma_device_type_playback) {
        LOG_ERROR("Invalid device type", "");

        return;
    }

    if (ma_device_is_started(&ctx->device)) {
        LOG_INFO("Playback device is already started", "");
        return;
    }

    ma_result startResult = ma_device_start(&ctx->device);

    if (startResult != MA_SUCCESS) {
        LOG_ERROR("Failed to start playback device - %s",
                  ma_result_description(startResult));

        return;
    }
}

FFI_PLUGIN_EXPORT
void playback_device_stop(void *pDevice) {
    if (pDevice == NULL) {
        LOG_ERROR("Invalid playback device", "");

        return;
    }

    playback_device_t *ctx = (playback_device_t *)pDevice;

    ma_result stopResult = ma_device_stop(&ctx->device);
    if (stopResult != MA_SUCCESS) {
        LOG_ERROR("Failed to stop playback device - %s",
                  ma_result_description(stopResult));

        return;
    }
}

FFI_PLUGIN_EXPORT
void playback_device_push_buffer(void *pDevice, playback_data_t *pData) {
    if (pDevice == NULL) {
        LOG_ERROR("Invalid playback device", "");
        return;
    }

    if (pData == NULL) {
        LOG_ERROR("Invalid playback data", "");
        return;
    }

    playback_device_t *ctx = (playback_device_t *)pDevice;
    ma_format format = ctx->device.playback.format;

    if (format != (ma_format)pData->format) {
        LOG_ERROR("Invalid format", "");
        return;
    }

    ma_uint32 availableWrite = ma_rb_available_write(&ctx->rb);
    ma_uint32 availableRead = ma_rb_available_read(&ctx->rb);
    size_t bufferSize = ma_rb_get_subbuffer_size(&ctx->rb);
    size_t sizeInBytes = pData->sizeInBytes;

    // Calculate buffer available write percentage
    float availableWritePercentage = (float)availableWrite / bufferSize * 100;

    // calculate buffer available write in seconds

    float timeInSec = (float)availableRead / (pData->format * 2 * 441000);

    if (availableWritePercentage > 10) {
        LOG_INFO("Buffer available write: %.2f%% (%.2f seconds)",
                 availableWritePercentage, timeInSec);
    }

    if (availableWrite < sizeInBytes) {
        size_t bytesToSkip = sizeInBytes - availableWrite;

        if (bytesToSkip > availableRead) {
            LOG_WARN("Not enough data in buffer to skip. Adjusting bytesToSkip to %u bytes.", availableRead);
            bytesToSkip = availableRead;
        }

        ma_result seekResult = ma_rb_seek_read(&ctx->rb, bytesToSkip);

        if (seekResult != MA_SUCCESS) {
            LOG_ERROR("Failed to seek read pointer: %s", ma_result_description(seekResult));
            return;
        }

        LOG_INFO(
            "Skipped %zu "
            "BS size: %u  "
            "[AR %u]  "
            "[AW %u] "
            "DS: %u",
            bytesToSkip,
            bufferSize,
            availableRead,
            availableWrite,
            sizeInBytes);
    }

    // Acquire write buffer
    void *bufferOut;

    ma_result writeResult =
        ma_rb_acquire_write(&ctx->rb, &sizeInBytes, &bufferOut);

    if (writeResult != MA_SUCCESS) {
        LOG_ERROR("Failed to acquire write buffer - %s",
                  ma_result_description(writeResult));

        return;
    }

    memcpy(bufferOut, pData->pUserData, sizeInBytes);

    ma_result commitResult = ma_rb_commit_write(&ctx->rb,
                                                sizeInBytes);

    if (commitResult != MA_SUCCESS) {
        LOG_ERROR("Failed to commit write buffer - %s",
                  ma_result_description(commitResult));
    }
}
