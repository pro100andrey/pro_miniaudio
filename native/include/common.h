#ifndef COMMON_H
#define COMMON_H

/**
 * @def MAX_DEVICE_NAME_LENGTH
 * @brief Defines the maximum length of a device name, including the null terminator.
 */
#define MAX_DEVICE_NAME_LENGTH 255

/**
 * @enum device_state
 * @brief Enumeration of possible states for a playback device.
 */
typedef enum {
    device_state_uninitialized = 0,
    device_state_stopped = 1,  /* The device's default state after initialization. */
    device_state_started = 2,  /* The device is started and is requesting and/or delivering audio data. */
    device_state_starting = 3, /* Transitioning from a stopped state to started. */
    device_state_stopping = 4  /* Transitioning from a started state to stopped. */
} device_state_t;

#endif  // !COMMON_H
