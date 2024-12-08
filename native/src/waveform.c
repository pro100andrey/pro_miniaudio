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

    ma_waveform_config config =
        ma_waveform_config_init((ma_format)format,
                                channels,
                                sampleRate,
                                type,
                                amplitude,
                                frequency);

    ma_waveform *waveform = (ma_waveform *)malloc(sizeof(ma_waveform));

    if (!waveform) {
        LOG_ERROR("failed to allocate memory for ma_waveform", "");
        return NULL;
    }

    ma_result waveformInitResult =
        ma_waveform_init(&config,
                         waveform);

    if (waveformInitResult != MA_SUCCESS) {
        free(waveform);

        LOG_ERROR("ma_waveform_init failed - %s",
                  ma_result_description(waveformInitResult));

        return NULL;
    }

    LOG_INFO("<%p>(ma_waveform) created.", waveform);

    return waveform;
}

FFI_PLUGIN_EXPORT
void waveform_destroy(void *pWaveform) {
    if (!pWaveform) {
        LOG_ERROR("invalid parameter: `pWaveform` is NULL", "");
        return;
    }

    ma_waveform *waveform = (ma_waveform *)pWaveform;

    ma_waveform_uninit(waveform);

    free(waveform);

    LOG_INFO("<%p>(ma_waveform) destroyed.", waveform);
}

FFI_PLUGIN_EXPORT
void waveform_read_pcm_frames_with_buffer(void *pWaveform,
                                          void *pFramesOut,
                                          uint64_t framesCount,
                                          uint64_t *pFramesRead) {
    if (!pWaveform) {
        LOG_ERROR("invalid parameter: `pWaveform` is NULL", "");

        return;
    }

    if (!pFramesOut) {
        LOG_ERROR("invalid parameter: `pFramesOut` is NULL", "");
        return;
    }

    if (!pFramesRead) {
        LOG_ERROR("invalid parameter: `pFramesRead` is NULL", "");
        return;
    }

    ma_waveform *waveform = (ma_waveform *)pWaveform;

    ma_result maWaveformReadResult =
        ma_waveform_read_pcm_frames(waveform,
                                    pFramesOut,
                                    framesCount,
                                    pFramesRead);

    if (maWaveformReadResult != MA_SUCCESS) {
        LOG_ERROR("ma_waveform_read_pcm_frames failed - %s",
                  ma_result_description(maWaveformReadResult));
    }
}
