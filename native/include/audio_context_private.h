#ifndef AUDIO_CONTEXT_PRIVATE_H
#define AUDIO_CONTEXT_PRIVATE_H

#include "miniaudio.h"
#include "playback_device_private.h"

/**
 * @struct audio_context_t
 * @brief Structure to manage the audio system context.
 *
 * The `audio_context_t` structure is the central manager for audio devices and
 * the Miniaudio context. It provides functionality to register, unregister,
 * and manage audio devices within the system.
 */
typedef struct {
    ma_context maContext;          /**< Miniaudio context for initializing the audio system. */
    audio_device_t **audioDevices; /**< Dynamic array of pointers to all audio devices. */
    size_t deviceCount;            /**< Number of devices currently managed by the context. */
} audio_context_t;

/**
 * @brief Registers a new audio device in the context.
 *
 * Adds a new audio device to the list of managed devices within the context.
 *
 * @param self Pointer to the `audio_context_t` structure.
 * @param pDevice Pointer to the `audio_device_t` structure to register.
 * @return `true` if the device was successfully registered, `false` otherwise.
 */
bool context_register_device(audio_context_t *self, audio_device_t *pDevice);

/**
 * @brief Unregisters an audio device from the context.
 *
 * Removes an audio device from the list of managed devices within the context.
 * If the device is found, it will be removed and the array will be updated.
 *
 * @param self Pointer to the `audio_context_t` structure.
 * @param pDevice Pointer to the `audio_device_t` structure to unregister.
 * @return `true` if the device was successfully unregistered, `false` otherwise.
 */
bool context_unregister_device(audio_context_t *self, audio_device_t *pDevice);

#endif  // AUDIO_CONTEXT_PRIVATE_H
