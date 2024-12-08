#ifndef PLAYBACK_DEVICE_H
#define PLAYBACK_DEVICE_H

#include "context.h"
#include "platform.h"

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
 * @param p Pointer to the `ma_context`.
 * @param bufferSizeInBytes Size of the internal buffer in bytes.
 * @param deviceId Identifier of the device to be used.
 * @param supportedFormat Format configuration for the playback device.
 * @return A pointer to the playback device.
 */
FFI_PLUGIN_EXPORT void *playback_device_create(const void *p,
                                               size_t bufferSizeInBytes,
                                               device_id_t deviceId,
                                               supported_format_t supportedFormat);

/**
 * @brief Destroys a playback device and releases its resources.
 *
 * @param p Pointer to the playback device to destroy.
 */
FFI_PLUGIN_EXPORT void playback_device_destroy(void *p);

/**
 * @brief Starts playback on the device.
 *
 * @param p Pointer to the playback device.
 */
FFI_PLUGIN_EXPORT void playback_device_start(void *p);

/**
 * @brief Stops playback on the device.
 *
 * @param p Pointer to the playback device.
 */
FFI_PLUGIN_EXPORT void playback_device_stop(void *p);

/**
 * @brief Pushes audio data to the playback device's buffer.
 *
 * @param p Pointer to the playback device.
 * @param pData Pointer to the data to be pushed.
 */
FFI_PLUGIN_EXPORT void playback_device_push_buffer(void *p, playback_data_t *pData);

#endif  // !PLAYBACK_DEVICE_H
