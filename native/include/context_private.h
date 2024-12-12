#ifndef CONTEXT_PRIVATE_H
#define CONTEXT_PRIVATE_H

#include "miniaudio.h"
#include "playback_device_private.h"

/**
 * @struct device_info_cache_t
 * @brief Structure to store cached device information.
 *
 * This structure is used to cache information about available audio devices.
 * It includes a count of devices and a pointer to an array of `device_info_t` structures
 * that describe each device.
 */
typedef struct {
    uint32_t count;            /**< Number of devices in the cache. */
    device_info_t *deviceInfo; /**< Pointer to an array of device information structures. */
} device_info_cache_t;

/**
 * @struct context_t
 * @brief Structure to manage the audio system context.
 *
 * The `context_t` structure is the central manager for audio devices and
 * the Miniaudio context. It provides functionality to register, unregister,
 * and manage audio devices within the system.
 */
typedef struct {
    ma_context maContext;              /**< Miniaudio context for initializing the audio system. */
    device_info_cache_t playbackCache; /**< Cache of playback devices. */
    device_info_cache_t captureCache;  /**< Cache of capture devices. */
    audio_device_t **audioDevices;     /**< Dynamic array of pointers to all audio devices. */
    size_t deviceCount;                /**< Number of devices currently managed by the context. */
} context_t;

/**
 * @brief Registers a new audio device in the context.
 *
 * Adds a new audio device to the list of managed devices within the context.
 *
 * @param pContext Pointer to the `context_t` structure.
 * @param pDevice Pointer to the `audio_device_t` structure to register.
 * @return `true` if the device was successfully registered, `false` otherwise.
 */
bool context_register_device(context_t *pContext, audio_device_t *pDevice);

/**
 * @brief Unregisters an audio device from the context.
 *
 * Removes an audio device from the list of managed devices within the context.
 * If the device is found, it will be removed and the array will be updated.
 *
 * @param pContext Pointer to the `context_t` structure.
 * @param pDevice Pointer to the `audio_device_t` structure to unregister.
 * @return `true` if the device was successfully unregistered, `false` otherwise.
 */
bool context_unregister_device(context_t *pContext, audio_device_t *pDevice);

#endif  // CONTEXT_PRIVATE_H
