#ifndef WAVEFORM_H
#define WAVEFORM_H

#include "context.h"
#include "platform.h"

/**
 * @enum waveform_type_t
 * @brief Represents the types of waveforms that can be generated.
 */
typedef enum {
    waveform_type_sine,     /**< Sine waveform. */
    waveform_type_square,   /**< Square waveform. */
    waveform_type_triangle, /**< Triangle waveform. */
    waveform_type_sawtooth  /**< Sawtooth waveform. */
} waveform_type_t;

/**
 * @brief Creates a waveform generator with the specified parameters.
 *
 * This function initializes a waveform generator that produces a continuous
 * audio signal based on the specified waveform type, amplitude, and frequency.
 *
 * @param pcmFormat The PCM format for the waveform (e.g., `pcm_format_f32`).
 * @param channels The number of audio channels (e.g., 1 for mono, 2 for stereo).
 * @param sampleRate The sample rate in Hertz (e.g., 44100 Hz).
 * @param waveformType The type of waveform to generate (e.g., sine, square).
 * @param amplitude The amplitude of the waveform, typically in the range [0.0, 1.0].
 * @param frequency The frequency of the waveform in Hertz.
 * @return A pointer to the waveform generator, or NULL if initialization fails.
 */
FFI_PLUGIN_EXPORT void *waveform_create(pcm_format_t pcmFormat,
                                        uint32_t channels,
                                        uint32_t sampleRate,
                                        waveform_type_t waveformType,
                                        double amplitude,
                                        double frequency);

/**
 * @brief Destroys a waveform generator and releases its resources.
 *
 * This function cleans up any memory or resources associated with the
 * waveform generator.
 *
 * @param waveform Pointer to the waveform generator to destroy.
 */
FFI_PLUGIN_EXPORT void waveform_destroy(void *waveform);

/**
 * @brief Reads PCM frames from the waveform generator.
 *
 * Generates the specified number of PCM audio frames and writes them
 * to the provided output buffer.
 *
 * @param waveform Pointer to the waveform generator.
 * @param pFramesOut Pointer to the output buffer where PCM frames will be written.
 * @param framesCount The number of frames to generate.
 * @param pFramesRead Pointer to a variable that will store the actual number of frames read.
 */
FFI_PLUGIN_EXPORT void waveform_read_pcm_frames_with_buffer(void *waveform,
                                                            void *pFramesOut,
                                                            uint64_t framesCount,
                                                            uint64_t *pFramesRead);

#endif  // WAVEFORM_H
