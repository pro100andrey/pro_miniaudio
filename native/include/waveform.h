#ifndef WAVEFORM_H
#define WAVEFORM_H

#include "audio_context.h"
#include "platform.h"
#include "result.h"

/**
 * @enum waveform_type_t
 * @brief Enum representing different types of waveforms.
 */
typedef enum {
    waveform_type_sine,     /* Sine waveform. */
    waveform_type_square,   /* Square waveform. */
    waveform_type_triangle, /* Triangle waveform. */
    waveform_type_sawtooth  /* Sawtooth waveform. */
} waveform_type_t;

/**
 * @brief Creates a waveform generator with the specified parameters.
 *
 * @param format Audio sample format.
 * @param channels Number of audio channels.
 * @param sampleRate Sample rate in Hertz.
 * @param waveformType Type of waveform to generate.
 * @param amplitude Amplitude of the waveform.
 * @param frequency Frequency of the waveform in Hertz.
 * @return A `result_t` structure containing a pointer to the waveform generator or an error code.
 */
FFI_PLUGIN_EXPORT result_t waveform_create(sample_format_t format,
                                           u_int32_t channels,
                                           uint32_t sampleRate,
                                           waveform_type_t waveformType,
                                           double amplitude,
                                           double frequency);

/**
 * @brief Destroys a waveform generator and releases its resources.
 *
 * @param waveform Pointer to the waveform generator to destroy.
 */
FFI_PLUGIN_EXPORT void waveform_destroy(void *waveform);

/**
 * @brief Reads PCM frames from the waveform generator.
 *
 * @param waveform Pointer to the waveform generator.
 * @param pFramesOut Pointer to the output buffer for PCM frames.
 * @param framesCount Number of frames to read.
 * @param pFramesRead Pointer to a variable to store the number of frames read.
 * @return A `result_t` structure indicating success or failure.
 */
FFI_PLUGIN_EXPORT result_t waveform_read_pcm_frames_with_buffer(void *waveform,
                                                                void *pFramesOut,
                                                                uint64_t framesCount,
                                                                uint64_t *pFramesRead);

#endif  // !WAVEFORM_H
