#ifndef PLAYBACK_DEVICE_H
#define PLAYBACK_DEVICE_H

#include "context.h"
#include "platform.h"

/**
 * @struct playback_data_t
 * @brief Represents audio data to be pushed to a playback device.
 *
 * This structure contains information about the audio data format,
 * a pointer to the data itself, and its size in bytes.
 */
typedef struct {
    void *pUserData;        /* Pointer to the audio data to be played. */
    uint32_t sizeInBytes;   /* Size of the audio data in bytes. */
} playback_data_t;

/**
 * @brief Creates a playback device with the specified parameters.
 *
 * Initializes a playback device for audio output with the given configuration.
 * Allocates internal buffers for audio processing.
 *
 * @param maContext Pointer to the `ma_context` used for audio device management.
 * @param bufferSizeInBytes Size of the internal audio buffer in bytes.
 * @param deviceId Identifier of the playback device to use.
 * @param supportedFormat Configuration of the supported audio format (sample rate, channels, etc.).
 * @return A pointer to the created playback device, or NULL if creation fails.
 */
FFI_PLUGIN_EXPORT void *playback_device_create(const void *maContext,
                                               size_t bufferSizeInBytes,
                                               device_id_t deviceId,
                                               supported_format_t supportedFormat);

/**
 * @brief Destroys a playback device and releases its resources.
 *
 * Stops any ongoing playback, deallocates internal buffers, and cleans up the device.
 *
 * @param pDevice Pointer to the playback device to destroy.
 */
FFI_PLUGIN_EXPORT void playback_device_destroy(void *pDevice);

/**
 * @brief Starts audio playback on the specified device.
 *
 * Begins playback using the audio data in the internal buffer.
 * Ensure that the buffer is pre-filled with valid audio data before calling this function.
 *
 * @param pDevice Pointer to the playback device.
 */
FFI_PLUGIN_EXPORT void playback_device_start(void *pDevice);

/**
 * @brief Stops audio playback on the specified device.
 *
 * Halts playback and clears any pending audio data in the buffer.
 *
 * @param pDevice Pointer to the playback device.
 */
FFI_PLUGIN_EXPORT void playback_device_stop(void *pDevice);

/**
 * @brief Pushes audio data into the playback device's buffer.
 *
 * Adds the specified audio data to the device's internal buffer for playback.
 * The data is copied into the buffer, so the caller retains ownership of the original data.
 *
 * @param pDevice Pointer to the playback device.
 * @param pData Pointer to a `playback_data_t` structure containing the audio data.
 */
FFI_PLUGIN_EXPORT void playback_device_push_buffer(void *pDevice, playback_data_t *pData);

#endif  // PLAYBACK_DEVICE_H
