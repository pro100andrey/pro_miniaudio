#ifndef PLAYBACK_DEVICE_H
#define PLAYBACK_DEVICE_H

#include "platform.h"
#include "result.h"
#include "audio_context.h"

/**
 * struct playback_data_t
 * @brief Structure to represent data being pushed to a playback device.
 */
typedef struct {
    sample_format_t format; /* Audio sample format. */
    void *pUserData;        /* Pointer to the audio data. */
    uint32_t sizeInBytes;   /* Size of the data in bytes. */
} playback_data_t;

/**
 * @brief Creates a playback device with the specified parameters.
 *
 * @param pAudioContext Pointer to the audio context.
 * @param bufferSizeInBytes Size of the internal buffer in bytes.
 * @param deviceId Identifier of the device to be used.
 * @param supportedFormat Format configuration for the playback device.
 * @return A `result_t` structure containing a pointer to the playback device or an error code.
 */
FFI_PLUGIN_EXPORT result_t playback_device_create(void *pAudioContext,
                                                  size_t bufferSizeInBytes,
                                                  device_id_t deviceId,
                                                  supported_format_t supportedFormat);

/**
 * @brief Destroys a playback device and releases its resources.
 *
 * @param device Pointer to the playback device to destroy.
 * @return A `result_t` structure indicating success or failure.
 */
FFI_PLUGIN_EXPORT result_t playback_device_destroy(void *device);

/**
 * @brief Starts playback on the device.
 *
 * @param device Pointer to the playback device.
 * @return A `result_t` structure indicating success or failure.
 */
FFI_PLUGIN_EXPORT result_t playback_device_start(void *device);

/**
 * @brief Stops playback on the device.
 *
 * @param device Pointer to the playback device.
 * @return A `result_t` structure indicating success or failure.
 */
FFI_PLUGIN_EXPORT result_t playback_device_stop(void *device);

/**
 * @brief Pushes audio data to the playback device's buffer.
 *
 * @param device Pointer to the playback device.
 * @param data Pointer to the data to be pushed.
 * @return A `result_t` structure indicating success or failure.
 */
FFI_PLUGIN_EXPORT result_t playback_device_push_buffer(void *device, playback_data_t *data);

#endif  // !PLAYBACK_DEVICE_H
