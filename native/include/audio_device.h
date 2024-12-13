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
    device_type_playback = 1,                                        /**< Playback device. */
    device_type_capture = 2,                                         /**< Capture device. */
    device_type_duplex = device_type_playback | device_type_capture, /**< Duplex device. */
    device_type_loopback = 4                                         /**< Loopback device. */
} audio_device_type_t;

typedef union {
    unsigned short wasapi[64];    /* WASAPI uses a wchar_t string for identification. */
    unsigned char dsound[16];     /* DirectSound uses a GUID for identification. */
    /*UINT_PTR*/ unsigned int winmm; /* When creating a device, WinMM expects a Win32 UINT_PTR for device identification. In practice it's actually just a UINT. */
    char alsa[256];               /* ALSA uses a name string for identification. */
    char pulse[256];              /* PulseAudio uses a name string for identification. */
    int jack;                     /* JACK always uses default devices. */
    char coreaudio[256];          /* Core Audio uses a string for identification. */
    char sndio[256];              /* "snd/0", etc. */
    char audio4[256];             /* "/dev/audio", etc. */
    char oss[64];                 /* "dev/dsp0", etc. "dev/dsp" for the default device. */
    signed int aaudio;            /* AAudio uses a 32-bit integer for identification. */
    unsigned int opensl;          /* OpenSL|ES uses a 32-bit unsigned integer for identification. */
    char webaudio[32];            /* Web Audio always uses default devices for now, but if this changes it'll be a GUID. */
    union {
        int i;
        char s[256];
        void *p;
    } custom;        /* The custom backend could be anything. Give them a few options. */
    int nullbackend; /* The null backend uses an integer for device IDs. */
} device_id;

/**
 * @struct audio_device_t
 * @brief Represents a generic audio device.
 */
typedef struct {
    audio_device_vtable_t *vtable; /**< Pointer to the virtual table for this device. */
    device_id id;                  /**< Unique identifier for the device. */
    void *owner;                   /**< Pointer to the owner context or object. */
    audio_device_type_t type;      /**< Type of the audio device. */
} audio_device_t;

/**
 * @brief Initializes a new audio device.
 *
 * @param dev Pointer to the audio device structure.
 * @param id Pointer to the unique identifier for the device.
 * @param owner Pointer to the owning context or object.
 * @param type Type of the audio device.
 */
void audio_device_create(audio_device_t *dev, device_id *id, void *owner, audio_device_type_t type);

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
