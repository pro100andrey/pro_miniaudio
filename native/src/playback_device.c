#include "../include/playback_device.h"

#include <stdlib.h>
#include <string.h>

#include "../include/internal.h"
#include "../include/logger.h"
#include "../include/miniaudio.h"

typedef struct {
    ma_device device;
    ma_rb rb;

    bool isReadingEnabled;
    size_t bufferSize;
    size_t midThreshold;
    size_t minThreshold;
} playback_device_t;

void data_callback(ma_device *pDevice,
                   void *pOutput,
                   const void *pInput,
                   ma_uint32 frameCount) {
    (void)pInput;

    playback_device_t *playback =
        (playback_device_t *)pDevice->pUserData;

    if (!playback) {
        LOG_ERROR("invalid parameter: `pDevice->pUserData` is NULL.\n", "");
        return;
    }

    if (!playback->isReadingEnabled) {
        LOG_DEBUG("Reading is disabled. Buffer not sufficiently filled.\n", "");
        return;
    }

    ma_uint32 bpf = ma_get_bytes_per_frame(pDevice->playback.format,
                                           pDevice->playback.channels);
    ma_uint32 availableRead =
        ma_rb_available_read(&playback->rb);

    if (availableRead < playback->minThreshold) {
        LOG_DEBUG("Reading is disabled. Buffer not sufficiently filled.\n", "");
        playback->isReadingEnabled = false;
        return;
    }

    if (availableRead == 0) {
        LOG_WARN("No data available for playback.\n", "");

        return;
    }

    ma_uint32 bytesPerFrames = frameCount * bpf;
    size_t bytesToRead = (availableRead < bytesPerFrames) ? availableRead : bytesPerFrames;

    while (bytesToRead > 0) {
        void *bufferOut;
        size_t chunkSize = bytesToRead;

        ma_result acquireReadResult =
            ma_rb_acquire_read(&playback->rb,
                               &chunkSize,
                               &bufferOut);
        if (acquireReadResult != MA_SUCCESS) {
            LOG_ERROR("`ma_rb_acquire_read` failed: %s.\n",
                      ma_result_description(acquireReadResult));
            break;
        }

        if (chunkSize == 0) {
            LOG_ERROR("No data available for playback.\n", "");
            break;
        }

        // Copy the data to the output buffer
        memcpy(pOutput, bufferOut, chunkSize);
        // Move the output pointer
        pOutput = (char *)pOutput + chunkSize;
        bytesToRead -= chunkSize;

        ma_result commitResult = ma_rb_commit_read(
            &playback->rb,
            chunkSize);

        if (commitResult == MA_AT_END) {
            LOG_WARN("`ma_rb_commit_read`: %s.\n",
                     ma_result_description(commitResult));
            playback->isReadingEnabled = false;
            LOG_DEBUG("Reading is disabled. Buffer not sufficiently filled.\n", "");
            break;
        } else if (commitResult != MA_SUCCESS) {
            LOG_ERROR("`ma_rb_commit_read`: %s.\n",
                      ma_result_description(commitResult));
            break;
        }
    }
}

void notification_callback(const ma_device_notification *pNotification) {
    switch (pNotification->type) {
        case ma_device_notification_type_started:
            LOG_INFO("playbackDevice started <%p>.\n", pNotification->pDevice);
            break;
        case ma_device_notification_type_stopped:
            LOG_INFO("playbackDevice stopped <%p>.\n", pNotification->pDevice);
            break;
        case ma_device_notification_type_rerouted:
            LOG_INFO("playbackDevice rerouted <%p>.\n", pNotification->pDevice);
            break;
        case ma_device_notification_type_interruption_began:
            LOG_INFO("playbackDevice interruption began <%p>.\n", pNotification->pDevice);
            break;
        case ma_device_notification_type_interruption_ended:
            LOG_INFO("playbackDevice interruption ended <%p>.\n", pNotification->pDevice);
            break;
        case ma_device_notification_type_unlocked:
            LOG_INFO("playbackDevice unlocked <%p>.\n", pNotification->pDevice);
            break;
        default:
            break;
    }
}

FFI_PLUGIN_EXPORT
void *playback_device_create(device_id_t deviceId,
                             audio_format_t audioFormat,
                             size_t bufferSizeInBytes) {
    if (bufferSizeInBytes == 0) {
        LOG_ERROR("`bufferSizeInBytes` must be greater than 0.\n", "");

        return NULL;
    }

    playback_device_t *playback = malloc(sizeof(playback_device_t));

    if (!playback) {
        LOG_ERROR("Failed to allocate memory for `playback_device_t`.\n", "");

        return NULL;
    }

    ma_device_id pDeviceId;
    memcpy(&pDeviceId, &deviceId, sizeof(deviceId));

    uint32_t channels = audioFormat.channels;
    uint32_t sampleRate = audioFormat.sampleRate;
    ma_format format = (ma_format)audioFormat.sampleFormat;

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
        ma_device_init(NULL,
                       &config,
                       &playback->device);

    if (maDeviceInitResult != MA_SUCCESS) {
        free(playback);

        LOG_ERROR("`ma_device_init` failed - %s.\n",
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
        ma_device_uninit(&playback->device);
        free(playback);

        LOG_ERROR("`ma_rb_init` failed - %s.\n",
                  ma_result_description(maRbInitResult));

        return NULL;
    }

    playback->bufferSize = bufferSizeInBytes;
    playback->midThreshold = bufferSizeInBytes / 2;
    playback->minThreshold = bufferSizeInBytes / 10;
    playback->isReadingEnabled = false;

    LOG_INFO(
        "<%p>(ma_device *) created - format: %s, channels: %d, sample_rate: %d.\n",
        &playback->device,
        describe_ma_format(format),
        channels,
        sampleRate);

    ma_uint32 bpf = ma_get_bytes_per_frame(format, channels);
    float fullBufferInSec = (float)bufferSizeInBytes / (sampleRate * bpf);

    LOG_INFO("<%p>(ma_rb *) created - buffer size: %zu (bytes) %f (sec).\n",
             &playback->rb,
             bufferSizeInBytes,
             fullBufferInSec);

    LOG_INFO("<%p>(playback_device_t *) created.\n",
             "buffer size: %zu (bytes) <%p>.\n",
             playback,
             bufferSizeInBytes,
             &playback);

    return playback;
}

FFI_PLUGIN_EXPORT
void playback_device_destroy(void *pDevice) {
    if (!pDevice) {
        LOG_ERROR("invalid parameter: `pDevice` is NULL.\n", "");
        return;
    }

    playback_device_t *playback = (playback_device_t *)pDevice;

    ma_result maDeviceStopResult =
        ma_device_stop(&playback->device);

    if (maDeviceStopResult != MA_SUCCESS) {
        LOG_WARN("`ma_device_stop` failed - %s.\n",
                 ma_result_description(maDeviceStopResult));
    }

    ma_rb_uninit(&playback->rb);
    LOG_INFO("<%p>(ma_rb *) destroyed.\n", &playback->rb);

    ma_device_uninit(&playback->device);
    LOG_INFO("<%p>(ma_device *) destroyed.\n", &playback->device);

    free(playback);
    LOG_INFO("<%p>(playback_device_t *) destroyed.\n", playback);
}

FFI_PLUGIN_EXPORT
void *playback_device_reset_buffer(void *pDevice) {
    if (!pDevice) {
        LOG_ERROR("invalid parameter: `pDevice` is NULL.\n", "");
        return NULL;
    }

    playback_device_t *playback = (playback_device_t *)pDevice;

    ma_rb_reset(&playback->rb);
    playback->isReadingEnabled = false;

    LOG_INFO("<%p>(ma_rb *) reset.\n", &playback->rb);

    return &playback->rb;
}

FFI_PLUGIN_EXPORT
void playback_device_start(void *pDevice) {
    if (!pDevice) {
        LOG_ERROR("invalid parameter: `pDevice` is NULL.\n", "");

        return;
    }

    playback_device_t *playback = (playback_device_t *)pDevice;

    if (playback->device.type != ma_device_type_playback) {
        LOG_ERROR("invalid playback type %d != %d.\n",
                  playback->device.type, ma_device_type_playback);

        return;
    }

    if (ma_device_is_started(&playback->device)) {
        LOG_INFO("playback <%p> already started.\n", playback);
        return;
    }

    ma_result maStartResult =
        ma_device_start(&playback->device);

    if (maStartResult != MA_SUCCESS) {
        LOG_ERROR("`ma_device_start` failed - %s.\n",
                  ma_result_description(maStartResult));

        return;
    }

    LOG_INFO("playback <%p> started.\n", playback);
}

FFI_PLUGIN_EXPORT
void playback_device_stop(void *pDevice) {
    if (!pDevice) {
        LOG_ERROR("invalid parameter: `pDevice` is NULL.\n", "");

        return;
    }

    playback_device_t *playbackDevice = (playback_device_t *)pDevice;

    ma_result stopResult =
        ma_device_stop(&playbackDevice->device);

    if (stopResult != MA_SUCCESS) {
        LOG_ERROR("`ma_device_stop` failed - %s.\n",
                  ma_result_description(stopResult));

        return;
    }

    LOG_INFO("playback <%p> stopped.\n", playbackDevice);
}

FFI_PLUGIN_EXPORT
void playback_device_push_buffer(void *pDevice, playback_data_t *pData) {
    if (!pDevice) {
        LOG_ERROR("invalid parameter: `pDevice` is NULL.\n", "");
        return;
    }

    if (!pData) {
        LOG_ERROR("invalid parameter: `pData` is NULL.\n", "");
        return;
    }

    if (pData->sizeInBytes == 0) {
        LOG_ERROR("invalid parameter: `pData->sizeInBytes` is 0.\n", "");
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

    LOG_DEBUG("Buffer: %.3fs, %.1f%%, %.3fs.\n",
              fullBufferInSec,
              bufferFillInPercent,
              bufferAvailableInSec);

    if (availableWrite < sizeInBytes) {
        size_t bytesToSkip = sizeInBytes - availableWrite;

        if (bytesToSkip > availableRead) {
            LOG_WARN(
                "Not enough space in the buffer to write data: "
                "skipping %zu bytes, available read: %zu.\n",
                bytesToSkip,
                availableRead);
            bytesToSkip = availableRead;
        }

        ma_result maRbSeekResult =
            ma_rb_seek_read(&playback->rb, bytesToSkip);

        if (maRbSeekResult != MA_SUCCESS) {
            LOG_ERROR("`ma_rb_seek_read` failed: %s.\n",
                      ma_result_description(maRbSeekResult));
            return;
        }
    }

    // Acquire write buffer
    void *bufferOut;

    ma_result writeResult =
        ma_rb_acquire_write(&playback->rb,
                            &sizeInBytes,
                            &bufferOut);

    if (writeResult != MA_SUCCESS) {
        LOG_ERROR("`ma_rb_acquire_write` failed - %s.\n",
                  ma_result_description(writeResult));

        return;
    }

    memcpy(bufferOut, pData->pUserData, sizeInBytes);

    ma_result maRbCommitResult =
        ma_rb_commit_write(&playback->rb,
                           sizeInBytes);

    if (maRbCommitResult != MA_SUCCESS) {
        LOG_ERROR("`ma_rb_commit_write` failed - %s.\n",
                  ma_result_description(maRbCommitResult));
    }

    if (!playback->isReadingEnabled && availableRead >= playback->midThreshold) {
        playback->isReadingEnabled = true;
        LOG_INFO("Buffer filled to %zu bytes. Reading enabled.\n", availableRead);
    }
}

FFI_PLUGIN_EXPORT
device_state_t playback_device_get_state(void *pDevice) {
    if (!pDevice) {
        LOG_ERROR("invalid parameter: `pDevice` is NULL.\n", "");
        return false;
    }

    playback_device_t *playback = (playback_device_t *)pDevice;

    ma_device_state state = ma_device_get_state(&playback->device);

    return (device_state_t)state;
}
