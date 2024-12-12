#ifndef __AUDIO_DEVICE_H__
#define __AUDIO_DEVICE_H__

/**
 * @enum device_state_t
 * @brief Represents the states of an audio device.
 */
typedef enum {
    device_state_uninitialized = 0, /**< The device is not initialized. */
    device_state_stopped = 1,       /**< The device is initialized but not started. */
    device_state_started = 2,       /**< The device is active and processing audio data. */
    device_state_starting = 3,      /**< The device is transitioning from stopped to started. */
    device_state_stopping = 4       /**< The device is transitioning from started to stopped. */
} device_state_t;

/**
 * @struct audio_device_vtable_t
 * @brief Virtual table for audio devices, providing function pointers for operations.
 */
typedef struct {
    void (*start)(void *self);               /**< Starts the audio device. */
    void (*stop)(void *self);                /**< Stops the audio device. */
    void (*destroy)(void *self);             /**< Destroys the audio device and releases its resources. */
    device_state_t (*get_state)(void *self); /**< Gets the current state of the audio device. */
} audio_device_vtable_t;

/**
 * @enum audio_device_type_t
 * @brief Defines the type of the audio device.
 */
typedef enum {
    audio_device_type_capture = 0, /**< A device for audio capture (input). */
    audio_device_type_playback     /**< A device for audio playback (output). */
} audio_device_type_t;

/**
 * @struct audio_device_t
 * @brief Represents a generic audio device.
 */
typedef struct {
    audio_device_vtable_t *vtable; /**< Pointer to the virtual table for this device. */
    void *id;                      /**< Unique identifier for the device. */
    void *owner;                   /**< Pointer to the owner context or object. */
} audio_device_t;

/**
 * @brief Initializes a new audio device.
 *
 * @param dev Pointer to the audio device structure.
 * @param id Pointer to the unique identifier for the device.
 * @param owner Pointer to the owning context or object.
 */
void audio_device_create(audio_device_t *dev, void *id, void *owner);

/**
 * @brief Starts the audio device.
 *
 * @param self Pointer to the audio device.
 */
void audio_device_start(void *self);

/**
 * @brief Stops the audio device.
 *
 * @param self Pointer to the audio device.
 */
void audio_device_stop(void *self);

/**
 * @brief Destroys the audio device and releases its resources.
 *
 * @param self Pointer to the audio device.
 */
void audio_device_destroy(void *self);

/**
 * @brief Gets the current state of the audio device.
 *
 * @param self Pointer to the audio device.
 * @return The current state of the audio device as a `device_state_t`.
 */
device_state_t audio_device_get_state(void *self);

/**
 * @brief Retrieves the base virtual table for audio devices.
 *
 * This function returns a pointer to the base virtual table, which can be used
 * to initialize generic audio devices or extend functionality in derived types.
 *
 * @return A pointer to the base `audio_device_vtable_t`.
 */
audio_device_vtable_t *audio_device_get_base_vtable(void);

#endif  // __AUDIO_DEVICE_H__
