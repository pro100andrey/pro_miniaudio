#include <assert.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/audio_context.h"
#include "../include/internal.h"
#include "../include/logger.h"
#include "../include/miniaudio.h"
#include "../include/playback_device.h"
#include "../include/waveform.h"

volatile bool running = true;

void handle_signal(int sig) {
    running = false;
}

int calculateFrameCount(uint32_t sampleRate, uint32_t milliseconds) {
    return sampleRate * milliseconds / 1000;
}

int main(int argc, char const* argv[]) {
    signal(SIGINT, handle_signal);

    set_log_to_console_enabled(true);
    set_log_level(LOG_LEVEL_INFO);

    void* pContext = audio_context_create();

    if (!pContext) {
        return 1;
    }

    audio_context_refresh_devices(pContext);

    uint32_t rPlaybackCount = audio_context_get_playback_device_count(pContext);
    device_info_t* pPlaybackDevices = audio_context_get_playback_device_infos(pContext);

    if (!pPlaybackDevices) {
        audio_context_destroy(pContext);
        return 1;
    }

    device_info_t playback = pPlaybackDevices[0];
    audio_format_t audioFormat = playback.audioFormats[0];

    playback_config_t config;
    config.channels = 1;
    config.sampleRate = 32000;
    config.pcmFormat = pcm_format_s16;

    uint32_t bpf = ma_get_bytes_per_frame((ma_format)config.pcmFormat, config.channels);
    uint32_t framesCount = calculateFrameCount(config.sampleRate, 100);
    uint32_t dataSizeInBytes = framesCount * bpf;
    size_t bufferSizeInBytes = dataSizeInBytes * 10;

    config.rbSizeInBytes = bufferSizeInBytes;
    config.rbMaxThreshold = bufferSizeInBytes / 2;
    config.rbMinThreshold = framesCount * 2;

    void* pPlaybackDevice = playback_device_create(pContext, playback.id, config);

    if (!pPlaybackDevice) {
        audio_context_destroy(pContext);

        return 1;
    }

    void* pWaveform =
        waveform_create(config.pcmFormat,
                        config.channels,
                        config.sampleRate,
                        waveform_type_sine,
                        1.0,
                        300);
    if (!pWaveform) {
        audio_context_destroy(pContext);

        return 1;
    }

    playback_data_t data = {
        .pUserData = malloc(dataSizeInBytes),
        .sizeInBytes = framesCount * bpf,
    };

    uint64_t pFramesRead = 0;

    waveform_read_pcm_frames_with_buffer(
        pWaveform,
        data.pUserData,
        framesCount,
        &pFramesRead);

    playback_device_push_buffer(pPlaybackDevice, &data);

    playback_device_start(pPlaybackDevice);

    device_state_t deviceState =
        playback_device_get_state(
            pPlaybackDevice);

    size_t tik = 0;

    while (running) {
        tik++;

        if (tik == 400) {
            break;
        }

        if (!data.pUserData) {
            break;
        }

        uint64_t pFramesRead = 0;

        waveform_read_pcm_frames_with_buffer(
            pWaveform,
            data.pUserData,
            framesCount,
            &pFramesRead);

        playback_device_push_buffer(pPlaybackDevice, &data);

        usleep(100000);
    }

    audio_context_destroy(pContext);
    playback_device_destroy(pPlaybackDevice);
    waveform_destroy(pWaveform);

    return 0;
}
