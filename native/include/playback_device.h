#ifndef PLAYBACK_DEVICE_H
#define PLAYBACK_DEVICE_H

#include "audio_context.h"
#include "audio_device.h"
#include "platform.h"

/**
 * @struct playback_config_t
 * @brief Configuration structure for a playback device.
 *
 * This structure defines the parameters required to initialize and configure
 * a playback device for audio output.
 */
typedef struct {
    uint32_t channels;      /**< Number of audio channels (e.g., 2 for stereo). */
    uint32_t sampleRate;    /**< Sample rate in Hertz (e.g., 44100 Hz). */
    pcm_format_t pcmFormat; /**< PCM format of the audio data (e.g., `pcm_format_s16`). */

    size_t rbMaxThreshold; /**< Maximum threshold for the ring buffer. */
    size_t rbMinThreshold; /**< Minimum threshold for the ring buffer. */
    size_t rbSizeInBytes;  /**< Total size of the ring buffer in bytes. */
} playback_config_t;

/**
 * @struct playback_data_t
 * @brief Represents audio data to be pushed to a playback device.
 *
 * This structure contains information about the audio data format,
 * a pointer to the data itself, and its size in bytes.
 */
typedef struct {
    void *pUserData;      /**< Pointer to the audio data to be played. */
    uint32_t sizeInBytes; /**< Size of the audio data in bytes. */
} playback_data_t;

/**
 * @brief Creates a playback device with the specified parameters.
 *
 * Initializes a playback device for audio output with the given configuration.
 * Allocates internal buffers for audio processing.
 *
 * @param pContext Pointer to the `audio_context_t` instance managing the audio devices.
 * @param pDeviceId Pointer to the device ID for the playback device.
 * @param config Configuration for the playback device, specified as a `playback_config_t`.
 * @return A pointer to the created playback device, or NULL if creation fails.
 */
FFI_PLUGIN_EXPORT
void *playback_device_create(void *pContext,
                             void *pDeviceId,
                             playback_config_t config);

/**
 * @brief Destroys a playback device and releases its resources.
 *
 * Stops any ongoing playback, deallocates internal buffers, and cleans up the device.
 *
 * @param self Pointer to the playback device to destroy.
 */
FFI_PLUGIN_EXPORT
void playback_device_destroy(void *self);

/**
 * @brief Resets the playback device's internal buffer.
 *
 * Clears any audio data currently in the buffer and ensures that the buffer is ready for new data.
 *
 * @param self Pointer to the playback device.
 */
FFI_PLUGIN_EXPORT
void playback_device_reset_buffer(void *self);

/**
 * @brief Starts audio playback on the specified device.
 *
 * Begins playback using the audio data in the internal buffer.
 * Ensure that the buffer is pre-filled with valid audio data before calling this function.
 *
 * @param self Pointer to the playback device.
 */
FFI_PLUGIN_EXPORT
void playback_device_start(void *self);

/**
 * @brief Stops audio playback on the specified device.
 *
 * Halts playback and clears any pending audio data in the buffer.
 *
 * @param self Pointer to the playback device.
 */
FFI_PLUGIN_EXPORT
void playback_device_stop(void *self);

/**
 * @brief Pushes audio data into the playback device's buffer.
 *
 * Adds the specified audio data to the device's internal buffer for playback.
 * The data is copied into the buffer, so the caller retains ownership of the original data.
 *
 * @param self Pointer to the playback device.
 * @param pData Pointer to a `playback_data_t` structure containing the audio data.
 */
FFI_PLUGIN_EXPORT
void playback_device_push_buffer(void *self, playback_data_t *pData);

/**
 * @brief Retrieves the current state of the playback device.
 *
 * Provides the current operational state of the playback device (e.g., started, stopped).
 *
 * @param self Pointer to the playback device.
 * @return The current state of the playback device, as a `device_state_t`.
 */
FFI_PLUGIN_EXPORT
device_state_t playback_device_get_state(void *self);

#endif  // PLAYBACK_DEVICE_H
