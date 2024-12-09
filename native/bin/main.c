#include <assert.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/context.h"
#include "../include/logger.h"
#include "../include/playback_device.h"
#include "../include/waveform.h"
#include "../include/miniaudio.h"

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
    set_log_level(LOG_LEVEL_DEBUG);

    void* pContext = context_create();

    if (!pContext) {
        return 1;
    }

    context_refresh_devices(pContext);

    uint32_t rPlaybackCount = context_get_playback_device_count(pContext);
    device_info_t* pPlaybackDevices = context_get_playback_device_infos(pContext);

    if (!pPlaybackDevices) {
        context_destroy(pContext);
        return 1;
    }

    device_info_t playback = pPlaybackDevices[0];
    audio_format_t audioFormat = playback.audioFormats[0];

    uint32_t channels = audioFormat.channels;
    uint32_t sampleRate = 8000;  // audioFormat.sampleRate;
    uint32_t sampleFormat = audioFormat.sampleFormat;

    uint32_t bpf =  ma_get_bytes_per_frame(sampleFormat, channels);
    uint32_t framesCount = calculateFrameCount(sampleRate, 100);

    uint32_t dataSizeInBytes = framesCount * bpf;
    size_t bufferSizeInBytes = dataSizeInBytes * 10;

    audio_format_t customAudioFormat;
    customAudioFormat.sampleFormat = sampleFormat;
    customAudioFormat.channels = channels;
    customAudioFormat.sampleRate = sampleRate;
    customAudioFormat.flags = 0;

    void* pPlaybackDevice = playback_device_create(
        playback.id,
        customAudioFormat,
        bufferSizeInBytes);

    if (!pPlaybackDevice) {
        context_destroy(pContext);

        return 1;
    }

    double amplitude = 1.0;
    double frequency = 300;

    void* pWaveform = waveform_create(sampleFormat,
                                      channels,
                                      sampleRate,
                                      waveform_type_sine,
                                      amplitude,
                                      frequency);
    if (!pWaveform) {
        context_destroy(pContext);

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

    device_state_t deviceState = playback_device_get_state(pPlaybackDevice);

    while (running) {
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

        usleep(98000);
    }

    context_destroy(pContext);
    playback_device_destroy(pPlaybackDevice);
    waveform_destroy(pWaveform);

    return 0;
}
