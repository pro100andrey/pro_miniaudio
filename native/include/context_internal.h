#ifndef CONTEXT_INTERNAL_H
#define CONTEXT_INTERNAL_H

#include <stdint.h>

#include "context.h"
#include "miniaudio.h"
#include "playback_device.h"
#include "playback_device_internal.h"
/**
 * @struct device_info_cache_t
 * @brief Structure to store device information.
 */
typedef struct {
    uint32_t count;
    device_info_t *deviceInfo;
} device_info_cache_t;

/**
 * @struct context_t
 * @brief Structure to manage the audio system context.
 */
typedef struct {
    ma_context maContext;               // Miniaudio context for audio system initialization
    device_info_cache_t playbackCache;  // Playback device cache
    device_info_cache_t captureCache;   // Capture device cache
} context_t;

#endif  // !CONTEXT_INTERNAL_H
