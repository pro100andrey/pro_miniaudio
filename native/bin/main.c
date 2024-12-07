#include <assert.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/audio_context.h"
#include "../include/logger.h"
#include "../include/playback_device.h"
#include "../include/result.h"
#include "../include/waveform.h"

volatile bool running = true;

void handle_signal(int sig) {
    running = false;
}

int main(int argc, char const* argv[]) {
    signal(SIGINT, handle_signal);

    set_log_to_console_enabled(true);

    result_t rContext = audio_context_create();
    if (rContext.code != error_code_none) {
        return 1;
    }

    void* pAudioContext = rContext.data.pData;

    result_t rRefresh = audio_context_refresh_devices(pAudioContext);
    if (rRefresh.code != error_code_none) {
        audio_context_destroy(pAudioContext);

        return 1;
    }

    result_t rPlaybackCount = audio_context_get_playback_device_count(pAudioContext);
    if (rPlaybackCount.code != error_code_none) {
        audio_context_destroy(pAudioContext);

        return 1;
    }

    int playbackCount = rPlaybackCount.data.intData;

    result_t rPlaybackDevices = audio_context_get_playback_devices_info(pAudioContext);
    if (rPlaybackDevices.code != error_code_none) {
        audio_context_destroy(pAudioContext);

        return 1;
    }

    const device_info_t* playbackDevices = rPlaybackDevices.data.pData;

    device_info_t playbackDevice = playbackDevices[0];

    supported_format_t supportedFormat = playbackDevice.dataFormats[0];
    uint32_t bpf = get_bytes_per_frame(supportedFormat.format, supportedFormat.channels);

    uint32_t framesCount = 4410;
    uint32_t dataSizeInBytes = framesCount * bpf;
    size_t bufferSizeInBytes = dataSizeInBytes * 10;

    result_t rPlaybackDevice = playback_device_create(
        pAudioContext,
        bufferSizeInBytes,
        playbackDevice.id,
        supportedFormat);

    if (rPlaybackDevice.code != error_code_none) {
        audio_context_destroy(pAudioContext);

        return 1;
    }

    double amplitude = 1.0;
    double frequency = 300;

    result_t formResult = waveform_create(
        supportedFormat.format,
        supportedFormat.channels,
        supportedFormat.sampleRate,
        waveform_type_sine,
        amplitude,
        frequency);

    if (formResult.code != error_code_none) {
        audio_context_destroy(pAudioContext);

        return 1;
    }

    void* pWaveform = formResult.data.pData;
    void* device = rPlaybackDevice.data.pData;

    result_t startResult = playback_device_start(device);

    if (startResult.code != error_code_none) {
        return 1;
    }

    int tik = 10;
    while (running) {
        tik--;

        if (tik == 0) {
            break;
        }

        playback_data_t data = {
            .format = supportedFormat.format,
            .pUserData = NULL,
            .sizeInBytes = framesCount * bpf,
        };

        void* userData = malloc(dataSizeInBytes);

        data.pUserData = userData;

        if (!data.pUserData) {
            break;
        }

        uint64_t pFramesRead = 0;

        result_t readResult = waveform_read_pcm_frames_with_buffer(
            pWaveform,
            data.pUserData,
            framesCount,
            &pFramesRead);

        if (readResult.code == error_code_none) {
            result_t pushResult = playback_device_push_buffer(device, &data);

            (void)pushResult;
        }

        free(data.pUserData);

        usleep(95000);
    }
    

    return 0;
}
