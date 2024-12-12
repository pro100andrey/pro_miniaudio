#ifndef PLAYBACK_DEVICE_PRIVATE_H
#define PLAYBACK_DEVICE_PRIVATE_H

#include "audio_device.h"
#include "miniaudio.h"
#include "playback_device.h"

/**
 * @struct playback_device_t
 * @brief Represents a playback audio device, derived from `audio_device_t`.
 *
 * This structure extends the base `audio_device_t` to include playback-specific
 * configurations, a Miniaudio device instance, and a ring buffer for audio data.
 */
typedef struct {
    audio_device_t base;      /**< Base audio device structure. */
    playback_config_t config; /**< Configuration for the playback device. */
    ma_device device;         /**< Miniaudio device for handling playback. */
    ma_rb rb;                 /**< Ring buffer for managing audio data. */
    bool isReadingEnabled;    /**< Indicates whether the playback device can read from the buffer. */
} playback_device_t;

#endif  // PLAYBACK_DEVICE_PRIVATE_H
