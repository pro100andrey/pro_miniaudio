#include "../include/waveform.h"

#include <stdlib.h>

#include "../include/logger.h"
#include "../include/miniaudio.h"

FFI_PLUGIN_EXPORT
void *waveform_create(sample_format_t format,
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
        LOG_ERROR("Failed to allocate memory for waveform", "");
        return NULL;
    }

    ma_result waveformInitResult = ma_waveform_init(&waveformConfig,
                                                    waveform);

    if (waveformInitResult != MA_SUCCESS) {
        free(waveform);

        LOG_ERROR("Failed to initialize waveform - %s",
                  ma_result_description(waveformInitResult));

        return NULL;
    }

    LOG_INFO("Waveform created <%p>.", waveform);

    return waveform;
}

FFI_PLUGIN_EXPORT
void waveform_destroy(void *pWaveform) {
    if (pWaveform == NULL) {
        LOG_ERROR("Invalid waveform provided for destruction", "");
        return;
    }

    ma_waveform * pWf = (ma_waveform *)pWaveform;

    ma_waveform_uninit(pWf);

    free(pWf);

    LOG_INFO("Waveform destroyed <%p>.", pWf);
}

FFI_PLUGIN_EXPORT
void waveform_read_pcm_frames_with_buffer(void *pWaveform,
                                          void *pFramesOut,
                                          uint64_t framesCount,
                                          uint64_t *pFramesRead) {
    if (pFramesRead == NULL) {
        LOG_ERROR("Invalid pointer to frames read", "");
    }

    if (pFramesOut == NULL) {
        LOG_ERROR("Invalid pointer to output buffer", "");
    }

    ma_waveform *pWF = (ma_waveform *)pWaveform;

    ma_result readResult =
        ma_waveform_read_pcm_frames(pWF,
                                    pFramesOut,
                                    framesCount,
                                    pFramesRead);

    if (readResult != MA_SUCCESS) {
        LOG_ERROR("Failed to read waveform PCM frames - %s",
                  ma_result_description(readResult));
    }
}
