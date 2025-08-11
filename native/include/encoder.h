#ifndef ENCODER_H
#define ENCODER_H

#include "audio_context.h"
#include "platform.h"

typedef struct {
    uint32_t channels;      /**< Number of audio channels (e.g., 2 for stereo). */
    uint32_t sampleRate;    /**< Sample rate in Hertz (e.g., 44100 Hz). */
    pcm_format_t pcmFormat; /**< PCM format of the audio data (e.g., `pcm_format_s16`). */
} encoder_config_t;

/**
 * @brief Creates a new encoder instance.
 *
 * Initializes an encoder for writing audio data to a file.
 *
 * @param path The path to the output file.
 * @param pConfig Pointer to the encoder configuration.
 * @return A pointer to the newly created encoder instance, or NULL if the creation failed.
 */
FFI_PLUGIN_EXPORT
void* encoder_create(const char* path, encoder_config_t* pConfig);

/**
 * @brief Destroys an encoder instance.
 *
 * Deallocates memory associated with the encoder instance.
 *
 * @param self Pointer to the encoder instance to be destroyed.
 */
FFI_PLUGIN_EXPORT
void encoder_destroy(void* self);

#endif  // ENCODER_H
