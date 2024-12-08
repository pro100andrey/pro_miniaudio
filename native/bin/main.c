#include <assert.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/context.h"
#include "../include/logger.h"
#include "../include/playback_device.h"
#include "../include/waveform.h"

volatile bool running = true;

void handle_signal(int sig) {
    running = false;
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

    device_info_t playbackDeviceInfo = pPlaybackDevices[0];
    supported_format_t supportedFormat = playbackDeviceInfo.dataFormats[0];
    uint32_t bpf = get_bytes_per_frame(supportedFormat.format, supportedFormat.channels);

    uint32_t framesCount = 4410;
    uint32_t dataSizeInBytes = framesCount * bpf;
    size_t bufferSizeInBytes = dataSizeInBytes * 5;

    void* pPlaybackDevice = playback_device_create(
        pContext,
        bufferSizeInBytes,
        playbackDeviceInfo.id,
        supportedFormat);

    if (!pPlaybackDevice) {
        context_destroy(pContext);

        return 1;
    }

    double amplitude = 1.0;
    double frequency = 300;

    void* pWaveform = waveform_create(
        supportedFormat.format,
        supportedFormat.channels,
        supportedFormat.sampleRate,
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

        usleep(96500);
    }

    playback_device_stop(pPlaybackDevice);

    playback_device_destroy(pPlaybackDevice);

    waveform_destroy(pWaveform);

    context_destroy(pContext);

    return 0;
}
