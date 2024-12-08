#include "../include/playback_device.h"

#include <stdlib.h>
#include <string.h>

#include "../include/logger.h"
#include "../include/miniaudio.h"

typedef struct {
    ma_device device;  // Playback playbackDevice
    ma_rb rb;          // Ring buffer
} playback_device_t;

void data_callback(ma_device *pDevice,
                   void *pOutput,
                   const void *pInput,
                   ma_uint32 frameCount) {
    (void)pInput;

    playback_device_t *playback =
        (playback_device_t *)pDevice->pUserData;

    if (!playback) {
        LOG_ERROR("`pDevice->pUserData` is NULL", "");
        return;
    }

    ma_uint32 bpf = ma_get_bytes_per_frame(pDevice->playback.format,
                                           pDevice->playback.channels);
    ma_uint32 availableRead =
        ma_rb_available_read(&playback->rb);

    if (availableRead == 0) {
        LOG_WARN("No data available for playback", "");

        return;
    }

    ma_uint32 bytesPerFrames = frameCount * bpf;
    size_t bytesToRead = (availableRead < bytesPerFrames) ? availableRead : bytesPerFrames;
    size_t totalProcessed = 0;

    while (bytesToRead > 0) {
        void *bufferOut;
        size_t chunkSize = bytesToRead;

        ma_result acquireReadResult =
            ma_rb_acquire_read(&playback->rb,
                               &chunkSize,
                               &bufferOut);
        if (acquireReadResult != MA_SUCCESS) {
            LOG_ERROR("`ma_rb_acquire_read` failed: %s",
                      ma_result_description(acquireReadResult));
            break;
        }

        if (chunkSize == 0) {
            LOG_ERROR("No data available for playback.");
            break;
        }

        // Copy the data to the output buffer
        memcpy(pOutput, bufferOut, chunkSize);
        // Move the output pointer
        pOutput = (char *)pOutput + chunkSize;

        totalProcessed += chunkSize;
        bytesToRead -= chunkSize;

        ma_result commitResult = ma_rb_commit_read(
            &playback->rb,
            chunkSize);

        if (commitResult == MA_AT_END) {
            LOG_WARN("`ma_rb_commit_read`: end of buffer reached", "");
            break;
        } else if (commitResult != MA_SUCCESS) {
            LOG_ERROR("failed to commit read: %s",
                      ma_result_description(commitResult));
            break;
        }
    }
}

void notification_callback(const ma_device_notification *pNotification) {
    switch (pNotification->type) {
        case ma_device_notification_type_started:
            LOG_INFO("playbackDevice started <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_stopped:
            LOG_INFO("playbackDevice stopped <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_rerouted:
            LOG_INFO("playbackDevice rerouted <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_interruption_began:
            LOG_INFO("playbackDevice interruption began <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_interruption_ended:
            LOG_INFO("playbackDevice interruption ended <%p>.", pNotification->pDevice);
            break;
        case ma_device_notification_type_unlocked:
            LOG_INFO("playbackDevice unlocked <%p>.", pNotification->pDevice);
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
        LOG_ERROR("`pMaContext` is NULL", "");

        return NULL;
    }

    if (bufferSizeInBytes == 0) {
        LOG_ERROR("`bufferSizeInBytes` must be greater than 0", "");

        return NULL;
    }

    ma_context *maContext = (ma_context *)pMaContext;
    playback_device_t *playback = malloc(sizeof(playback_device_t));

    if (!playback) {
        LOG_ERROR("Failed to allocate memory for playback_device_t", "");

        return NULL;
    }

    ma_device_id pDeviceId;
    memcpy(&pDeviceId, &deviceId, sizeof(deviceId));

    uint32_t channels = supportedFormat.channels;
    uint32_t sampleRate = supportedFormat.sampleRate;
    ma_format format = (ma_format)supportedFormat.format;

    // Initialize the playback playbackDevice
    ma_device_config config =
        ma_device_config_init(ma_device_type_playback);

    config.playback.format = format;
    config.playback.channels = channels;
    config.playback.pDeviceID = &pDeviceId;
    config.sampleRate = sampleRate;
    config.dataCallback = data_callback;
    config.notificationCallback = notification_callback;
    config.pUserData = playback;

    ma_result maDeviceInitResult =
        ma_device_init(maContext,
                       &config,
                       &playback->device);

    if (maDeviceInitResult != MA_SUCCESS) {
        free(playback);

        LOG_ERROR("ma_device_init failed - %s",
                  ma_result_description(maDeviceInitResult));

        return NULL;
    }

    // Initialize the ring buffer
    ma_result maRbInitResult =
        ma_rb_init(bufferSizeInBytes,
                   NULL,
                   NULL,
                   &playback->rb);

    if (maRbInitResult != MA_SUCCESS) {
        free(playback);

        LOG_ERROR("`ma_rb_init` failed - %s",
                  ma_result_description(maRbInitResult));

        return NULL;
    }

    LOG_INFO("<%p>(ma_device *) created.", &playback->device);
    LOG_INFO("<%p>(ma_rb *) created.", &playback->rb);
    LOG_INFO("<%p>(playback_device_t *) created.",
             "buffer size: %zu (bytes) <%p>.",
             playback,
             bufferSizeInBytes,
             &playback);

    return playback;
}

FFI_PLUGIN_EXPORT
void playback_device_destroy(void *pDevice) {
    if (!pDevice) {
        LOG_ERROR("`pDevice` is NULL", "");
        return;
    }

    playback_device_t *playback = (playback_device_t *)pDevice;

    ma_result maDeviceStopResult =
        ma_device_stop(&playback->device);

    if (maDeviceStopResult != MA_SUCCESS) {
        LOG_WARN("`ma_device_stop` failed - %s",
                 ma_result_description(maDeviceStopResult));
    }

    ma_device_uninit(&playback->device);
    LOG_INFO("<%p>(ma_device *) destroyed.", &playback->device);

    ma_rb_uninit(&playback->rb);
    LOG_INFO("<%p>(ma_rb *) destroyed.", &playback->rb);

    free(playback);
    LOG_INFO("<%p>(playback_device_t *) destroyed.", playback);
}

FFI_PLUGIN_EXPORT void playback_device_start(void *pDevice) {
    if (!pDevice) {
        LOG_ERROR("`pDevice` is NULL", "");

        return;
    }

    playback_device_t *playback = (playback_device_t *)pDevice;

    if (playback->device.type != ma_device_type_playback) {
        LOG_ERROR("invalid playback type %d != %d",
                  playback->device.type, ma_device_type_playback);

        return;
    }

    if (ma_device_is_started(&playback->device)) {
        LOG_INFO("playback <%p> already started", playback);
        return;
    }

    ma_result maStartResult =
        ma_device_start(&playback->device);

    if (maStartResult != MA_SUCCESS) {
        LOG_ERROR("`ma_device_start` failed - %s",
                  ma_result_description(maStartResult));

        return;
    }
}

FFI_PLUGIN_EXPORT
void playback_device_stop(void *pDevice) {
    if (!pDevice) {
        LOG_ERROR("`pDevice` is NULL", "");

        return;
    }

    playback_device_t *playbackDevice = (playback_device_t *)pDevice;

    ma_result stopResult =
        ma_device_stop(&playbackDevice->device);

    if (stopResult != MA_SUCCESS) {
        LOG_ERROR("`ma_device_stop` failed - %s",
                  ma_result_description(stopResult));

        return;
    }
}

FFI_PLUGIN_EXPORT
void playback_device_push_buffer(void *pDevice, playback_data_t *pData) {
    if (!pDevice) {
        LOG_ERROR("`pDevice` is NULL", "");
        return;
    }

    if (!pData) {
        LOG_ERROR("`pData` is NULL", "");
        return;
    }

    if (pData->sizeInBytes == 0) {
        LOG_ERROR("`pData` is empty", "");
        return;
    }

    playback_device_t *playback = (playback_device_t *)pDevice;

    ma_uint32 availableWrite = ma_rb_available_write(&playback->rb);
    ma_uint32 availableRead = ma_rb_available_read(&playback->rb);
    size_t bufferSize = ma_rb_get_subbuffer_size(&playback->rb);
    size_t sizeInBytes = pData->sizeInBytes;
    ma_format format = playback->device.playback.format;
    ma_uint32 channels = playback->device.playback.channels;
    ma_uint32 sampleRate = playback->device.sampleRate;

    size_t bpf = ma_get_bytes_per_frame(format, channels);

    float bufferAvailableInPercent = (float)availableWrite / bufferSize * 100;
    float bufferAvailableInSec = (float)availableWrite / (sampleRate * bpf);
    float fullBufferInSec = (float)bufferSize / (sampleRate * bpf);
    float bufferFillInPercent = 100.0f - bufferAvailableInPercent;
    float bufferFillInSec = fullBufferInSec - bufferAvailableInSec;

    // Логирование метрик
    LOG_STATS(
        "buffer: "
        "full: %.2fs, "
        "available: %.2f%% (%.2fs), "
        "filled: %.2f%% (%.2fs)",
        fullBufferInSec,
        bufferAvailableInPercent,
        bufferAvailableInSec,
        bufferFillInPercent,
        bufferFillInSec);

    if (availableWrite < sizeInBytes) {
        size_t bytesToSkip = sizeInBytes - availableWrite;

        if (bytesToSkip > availableRead) {
            LOG_WARN(
                "Not enough space in the buffer to write data: "
                "skipping %zu bytes, available read: %zu",
                bytesToSkip,
                availableRead);
            bytesToSkip = availableRead;
        }

        ma_result maRbSeekResult =
            ma_rb_seek_read(&playback->rb, bytesToSkip);

        if (maRbSeekResult != MA_SUCCESS) {
            LOG_ERROR("`ma_rb_seek_read` failed: %s",
                      ma_result_description(maRbSeekResult));
            return;
        }

        LOG_STATS(
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
        ma_rb_acquire_write(&playback->rb,
                            &sizeInBytes,
                            &bufferOut);

    if (writeResult != MA_SUCCESS) {
        LOG_ERROR("`ma_rb_acquire_write` failed - %s",
                  ma_result_description(writeResult));

        return;
    }

    memcpy(bufferOut, pData->pUserData, sizeInBytes);

    ma_result maRbCommitResult =
        ma_rb_commit_write(&playback->rb,
                           sizeInBytes);

    if (maRbCommitResult != MA_SUCCESS) {
        LOG_ERROR("`ma_rb_commit_write` failed - %s",
                  ma_result_description(maRbCommitResult));
    }
}
