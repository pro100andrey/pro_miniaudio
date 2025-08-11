#include "../include/playback_device.h"

#include <stdlib.h>
#include <string.h>

#include "../include/audio_context_private.h"
#include "../include/internal.h"
#include "../include/logger.h"
#include "../include/miniaudio.h"
#include "../include/playback_device_private.h"

// Playback device vtable
typedef struct {
    audio_device_vtable_t base;
    void (*pushBuffer)(void *self, playback_data_t *pData);
    void (*resetBuffer)(void *self);
} playback_device_vtable_t;

static playback_device_vtable_t g_playback_device_vtable = {
    .base = {
        .start = playback_device_start,
        .stop = playback_device_stop,
        .destroy = playback_device_destroy,
        .get_state = playback_device_get_state},
    .pushBuffer = playback_device_push_buffer,
    .resetBuffer = playback_device_reset_buffer};

static void _encode(playback_device_t *playback, void *pData, ma_uint64 framesCount) {
    ma_encoder *encoder = (ma_encoder *)playback->encoder;

    if (encoder) {
        ma_uint64 writeFramesCount = framesCount;
        ma_uint64 framesWritten = framesCount;

        while (writeFramesCount > 0) {
            ma_encoder_write_pcm_frames(encoder,
                                        pData,
                                        writeFramesCount,
                                        &framesWritten);

            writeFramesCount -= framesWritten;

            if (writeFramesCount != 0) {
                pData = (char *)pData + framesWritten;
            }
        }
    }
}

// Playback device data callback
static void _data_callback(ma_device *pDevice,
                           void *pOutput,
                           const void *pInput,
                           ma_uint32 frameCount) {
    (void)pInput;

    playback_device_t *playback = (playback_device_t *)pDevice->pUserData;

    if (!playback) {
        LOG_ERROR("invalid parameter: `pDevice->pUserData` is NULL.\n", "");
        return;
    }

    if (!playback->isReadingEnabled) {
        LOG_DEBUG("Reading is disabled. Buffer not sufficiently filled.\n", "");
        _encode(playback, pOutput, frameCount);
        return;
    }

    ma_uint32 availableRead = ma_rb_available_read(&playback->rb);

    if (availableRead < playback->config.rbMinThreshold) {
        LOG_DEBUG("Reading is disabled. Buffer not sufficiently filled.\n", "");
        playback->isReadingEnabled = false;
        _encode(playback, pOutput, frameCount);
        return;
    }

    if (availableRead == 0) {
        LOG_WARN("No data available for playback.\n", "");
        _encode(playback, pOutput, frameCount);
        return;
    }
    ma_uint32 bpf =
        ma_get_bytes_per_frame((ma_format)playback->config.pcmFormat,
                               playback->config.channels);

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

        _encode(playback, bufferOut, chunkSize / bpf);
        // Move the input pointer
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
void *playback_device_create(void *pContext,
                             device_id *pDeviceId,
                             playback_config_t *pConfig,
                             void *pEncoder) {
    if (!pContext) {
        LOG_ERROR("invalid parameter: `pContext` is NULL.\n", "");
        return NULL;
    }

    if (!pDeviceId) {
        LOG_WARN("`pDeviceId` is NULL. Using default device.\n", "");
    }

    if (!pConfig) {
        LOG_ERROR("invalid parameter: `pConfig` is NULL.\n", "");
        return NULL;
    }

    playback_device_t *playback = malloc(sizeof(playback_device_t));

    playback->encoder = NULL;

    if (!playback) {
        LOG_ERROR("Failed to allocate memory for `playback_device_t`.\n", "");

        return NULL;
    }

    if (pEncoder) {
        LOG_INFO("Using ma_encoder <%p>.\n", pEncoder);
        playback->encoder = pEncoder;
    }

    LOG_INFO("<%p>(playback_device_t *) creating.\n", playback);

    // Copy the config
    memcpy(&playback->config, pConfig, sizeof(playback_config_t));
    uint32_t bpf = ma_get_bytes_per_frame((ma_format)pConfig->pcmFormat,
                                          pConfig->channels);

    float bufferSizeInSec =
        (float)pConfig->rbSizeInBytes / (pConfig->sampleRate * bpf);

    LOG_INFO("Config.\n", "");
    LOG_INFO("  format: %s\n", describe_ma_format((ma_format)pConfig->pcmFormat));
    LOG_INFO("  channels: %d\n", pConfig->channels);
    LOG_INFO("  sampleRate: %d\n", pConfig->sampleRate);
    LOG_INFO("  rbSizeInBytes: %d\n", pConfig->rbSizeInBytes);
    LOG_INFO("  rbMaxThreshold: %d\n", pConfig->rbMaxThreshold);
    LOG_INFO("  rbMinThreshold: %d\n", pConfig->rbMinThreshold);
    LOG_INFO("  bufferSizeInSec: %f\n", bufferSizeInSec);

    // Initialize the playback playbackDevice
    ma_device_config deviceConfig =
        ma_device_config_init(ma_device_type_playback);

    deviceConfig.playback.pDeviceID = (ma_device_id *)pDeviceId;
    deviceConfig.playback.format = (ma_format)pConfig->pcmFormat;
    deviceConfig.playback.channels = pConfig->channels;
    deviceConfig.sampleRate = pConfig->sampleRate;

    deviceConfig.dataCallback = _data_callback;
    deviceConfig.notificationCallback = notification_callback;
    deviceConfig.pUserData = playback;

    deviceConfig.opensl.enableCompatibilityWorkarounds = MA_TRUE;
    deviceConfig.opensl.recordingPreset = ma_opensl_recording_preset_voice_communication;

    deviceConfig.aaudio.enableCompatibilityWorkarounds = MA_TRUE;
    deviceConfig.aaudio.inputPreset = ma_aaudio_input_preset_voice_communication;

    audio_context_t *context = (audio_context_t *)pContext;

    ma_result maDeviceInitResult =
        ma_device_init(&context->maContext,
                       &deviceConfig,
                       &playback->device);

    if (maDeviceInitResult != MA_SUCCESS) {
        free(playback);

        LOG_ERROR("`ma_device_init` failed - %s.\n",
                  ma_result_description(maDeviceInitResult));

        return NULL;
    }

    // Initialize the ring buffer
    ma_result maRbInitResult =
        ma_rb_init(pConfig->rbSizeInBytes,
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

    playback->isReadingEnabled = false;

    audio_device_create(&playback->base, pDeviceId, context, device_type_playback);
    playback->base.vtable = (audio_device_vtable_t *)&g_playback_device_vtable;

    context_register_device(context, (audio_device_t *)playback);

    LOG_INFO("<%p>(ma_device *) created\n", &playback->device);
    LOG_INFO("<%p>(ma_rb *) created \n", &playback->rb);
    LOG_INFO("<%p>(playback_device_t *) created\n", playback);

    return playback;
}

FFI_PLUGIN_EXPORT
void playback_device_destroy(void *self) {
    if (!self) {
        LOG_ERROR("invalid parameter: `self` is NULL.\n", "");
        return;
    }

    playback_device_t *playback = (playback_device_t *)self;
    audio_context_t *pContext = (audio_context_t *)playback->base.owner;

    if (!pContext) {
        LOG_WARN("`pContext` is NULL. Skipping device unregistration and destroy.\n", "");
        return;
    }

    if (playback->base.vtable) {
        context_unregister_device(pContext, (audio_device_t *)playback);
    }

    playback->base.vtable = NULL;
    playback->base.owner = NULL;

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
device_state_t playback_device_get_state(void *self) {
    if (!self) {
        LOG_ERROR("invalid parameter: `self` is NULL.\n", "");
        return false;
    }

    playback_device_t *playback = (playback_device_t *)self;

    ma_device_state state = ma_device_get_state(&playback->device);

    return (device_state_t)state;
}

FFI_PLUGIN_EXPORT
void playback_device_start(void *self) {
    if (!self) {
        LOG_ERROR("invalid parameter: `self` is NULL.\n", "");

        return;
    }

    playback_device_t *playback = (playback_device_t *)self;

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
void playback_device_stop(void *self) {
    if (!self) {
        LOG_ERROR("invalid parameter: `self` is NULL.\n", "");

        return;
    }

    playback_device_t *playback = (playback_device_t *)self;

    ma_result stopResult =
        ma_device_stop(&playback->device);

    if (stopResult != MA_SUCCESS) {
        LOG_ERROR("`ma_device_stop` failed - %s.\n",
                  ma_result_description(stopResult));

        return;
    }

    LOG_INFO("playback <%p> stopped.\n", playback);
}

FFI_PLUGIN_EXPORT
void playback_device_push_buffer(void *self, playback_data_t *pData) {
    if (!self) {
        LOG_ERROR("invalid parameter: `self` is NULL.\n", "");
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

    playback_device_t *playback = (playback_device_t *)self;

    ma_uint32 availableWrite = ma_rb_available_write(&playback->rb);
    ma_uint32 availableRead = ma_rb_available_read(&playback->rb);
    size_t bufferSize = ma_rb_get_subbuffer_size(&playback->rb);
    size_t sizeInBytes = pData->sizeInBytes;

    ma_format format = (ma_format)playback->config.pcmFormat;
    ma_uint32 channels = playback->config.channels;
    ma_uint32 sampleRate = playback->config.sampleRate;
    size_t bpf = ma_get_bytes_per_frame(format, channels);

    float bufferAvailableInPercent = (float)availableWrite / bufferSize * 100;
    float bufferAvailableInSec = (float)availableWrite / (sampleRate * bpf);
    float fullBufferInSec = (float)bufferSize / (sampleRate * bpf);
    float bufferFillInPercent = 100.0f - bufferAvailableInPercent;
    float pushBufferInSec = (float)sizeInBytes / (sampleRate * bpf);

    LOG_DEBUG("rb: %.2fs, fill: %.2f%%, available: %.2fs, pData: %.3fs \n",
              fullBufferInSec,
              bufferFillInPercent,
              bufferAvailableInSec,
              pushBufferInSec);

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

    if (!playback->isReadingEnabled && availableRead >= playback->config.rbMaxThreshold) {
        playback->isReadingEnabled = true;
        LOG_INFO("rb filled to %zu bytes. Reading enabled.\n", availableRead);
    }
}

FFI_PLUGIN_EXPORT
void playback_device_reset_buffer(void *self) {
    if (!self) {
        LOG_ERROR("invalid parameter: `self` is NULL.\n", "");
        return;
    }

    playback_device_t *playback = (playback_device_t *)self;

    ma_rb_reset(&playback->rb);
    playback->isReadingEnabled = false;

    LOG_INFO("<%p>(ma_rb *) reset.\n", &playback->rb);

    return;
}
