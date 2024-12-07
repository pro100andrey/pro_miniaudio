#include "../include/waveform.h"

#include <stdlib.h>

#include "../include/logger.h"
#include "../include/miniaudio.h"

FFI_PLUGIN_EXPORT
result_t waveform_create(sample_format_t format,
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
        return result_error(
            error_waveform,
            "Failed to allocate memory for waveform");
    }

    ma_result waveformInitResult = ma_waveform_init(&waveformConfig,
                                                    waveform);

    if (waveformInitResult != MA_SUCCESS) {
        free(waveform);

        return result_error(
            error_waveform,
            "Failed to create waveform - %s",
            ma_result_description(waveformInitResult));
    }

    LOG_INFO("Waveform created <%p>.", waveform);

    return result_ptr(waveform);
}

FFI_PLUGIN_EXPORT
void waveform_destroy(void *waveform) {
    ma_waveform *pWaveform = (ma_waveform *)waveform;

    ma_waveform_uninit(pWaveform);

    free(pWaveform);

    LOG_INFO("Waveform destroyed <%p>.", waveform);
}

FFI_PLUGIN_EXPORT
result_t waveform_read_pcm_frames_with_buffer(void *waveform,
                                              void *pFramesOut,
                                              uint64_t framesCount,
                                              uint64_t *pFramesRead) {
    if (waveform == NULL || pFramesOut == NULL) {
        return result_error(error_waveform,
                            "Invalid parameters");
    }

    ma_waveform *pWaveform = (ma_waveform *)waveform;

    ma_result readResult =
        ma_waveform_read_pcm_frames(pWaveform,
                                    pFramesOut,
                                    framesCount,
                                    pFramesRead);

    if (readResult != MA_SUCCESS) {
        return result_error(
            error_waveform,
            "Failed to read waveform PCM frames - %s",
            ma_result_description(readResult));
    }

    return (result_t){0};
}
