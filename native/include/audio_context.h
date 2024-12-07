#ifndef AUDIO_CONTEXT_H
#define AUDIO_CONTEXT_H

#include <stdbool.h>

#include "platform.h"
#include "result.h"

/**
 * union device_id_t
 * @brief Union to store device identifiers for different audio backends.
 */
typedef union {
    unsigned short wasapi[64]; /* WASAPI uses a wchar_t string for identification. */
    unsigned char dsound[16];  /* DirectSound uses a GUID for identification. */
    unsigned int winmm;        /* WinMM expects a UINT_PTR for device identification. */
    char alsa[256];            /* ALSA uses a name string for identification. */
    char pulse[256];           /* PulseAudio uses a name string for identification. */
    int jack;                  /* JACK always uses default devices. */
    char coreaudio[256];       /* Core Audio uses a string for identification. */
    char sndio[256];           /* Sndio uses "snd/0", etc. */
    char audio4[256];          /* Audio4 uses "/dev/audio", etc. */
    char oss[64];              /* OSS uses "dev/dsp0", etc. */
    signed int aaudio;         /* AAudio uses a 32-bit integer for identification. */
    unsigned int opensl;       /* OpenSL|ES uses a 32-bit unsigned integer. */
    char webaudio[32];         /* WebAudio uses default devices. */
    union {
        int i;       /* Custom backend integer identifier. */
        char s[256]; /* Custom backend string identifier. */
        void *p;     /* Custom backend pointer identifier. */
    } custom;        /* Identifier for custom backends. */
    int nullbackend; /* Null backend uses an integer identifier. */
} device_id_t;

/**
 * @enum sample_format_t
 * @brief Enum to represent supported audio sample formats.
 */
typedef enum {
    sample_format_unknown = 0, /* Unknown or unsupported format. */
    sample_format_u8 = 1,      /* Unsigned 8-bit integer. */
    sample_format_s16 = 2,     /* Signed 16-bit integer. */
    sample_format_s24 = 3,     /* Signed 24-bit integer. */
    sample_format_s32 = 4,     /* Signed 32-bit integer. */
    sample_format_f32 = 5,     /* 32-bit floating point. */
    sample_format_count        /* Number of supported formats. */
} sample_format_t;

/**
 * struct supported_format_t
 * @brief Structure to describe a supported audio data format.
 */
typedef struct {
    sample_format_t format; /* Audio sample format. */
    uint32_t channels;      /* Number of channels. */
    uint32_t sampleRate;    /* Sample rate in Hertz. */
    uint32_t flags;         /* Format flags (reserved for future use). */
} supported_format_t;

/**
 * @def MAX_DEVICE_NAME_LENGTH
 * @brief Defines the maximum length of a device name, including the null terminator.
 */
#define MAX_DEVICE_NAME_LENGTH 255

/**
 * struct device_info_t
 * @brief Structure to describe an audio device.
 */
typedef struct {
    device_id_t id;                        /* Unique identifier for the device. */
    char name[MAX_DEVICE_NAME_LENGTH + 1]; /* Device name (null-terminated string). */
    bool isDefault;                        /* Indicates if this is the default device. */
    uint32_t dataFormatCount;              /* Number of supported formats. */
    supported_format_t dataFormats[64];    /* Array of supported audio formats. */
} device_info_t;

/**
 * @brief Creates a new Context and initializes the audio system with default settings.
 *
 * This function initializes the audio backend and prepares the system for audio device management.
 *
 * @return A `result_t` structure containing a pointer to the Context on success,
 *         or an error code and message on failure.
 */
FFI_PLUGIN_EXPORT result_t audio_context_create(void);

/**
 * @brief Checks if the provided Context is valid.
 *
 * This function verifies that the provided Context is not NULL and has been initialized.
 *
 * @param context Pointer to the Context to check.
 * @return `true` if the Context is valid, `false` otherwise.
 */
FFI_PLUGIN_EXPORT bool audio_context_is_valid(const void *context);

/**
 * @brief Destroys the Context and releases all associated resources.
 *
 * This function cleans up memory and system resources allocated by the Context.
 *
 * @param context Pointer to the Context to be destroyed. Passing NULL is safe and has no effect.
 * @return A `result_t` structure indicating success or failure.
 */
FFI_PLUGIN_EXPORT void audio_context_destroy(void *context);

/**
 * @brief Refreshes the list of available audio devices.
 *
 * Queries the system for the current list of playback and capture devices,
 * updating the internal device list in the provided Context.
 *
 * @param context Pointer to the Context to refresh devices for.
 * @return A `result_t` structure indicating success or failure.
 */
FFI_PLUGIN_EXPORT result_t audio_context_refresh_devices(const void *context);

/**
 * @brief Gets the number of available playback devices.
 *
 * @param context Pointer to the Context.
 * @return A `result_t` structure containing the number of playback devices on success,
 *         or an error code and message on failure.
 */
FFI_PLUGIN_EXPORT result_t audio_context_get_playback_device_count(const void *context);

/**
 * @brief Gets the number of available capture devices.
 *
 * @param context Pointer to the Context.
 * @return A `result_t` structure containing the number of capture devices on success,
 *         or an error code and message on failure.
 */
FFI_PLUGIN_EXPORT result_t audio_context_get_capture_device_count(const void *context);

/**
 * @brief Gets information about available playback devices.
 *
 * @param context Pointer to the Context.
 * @return A `result_t` structure containing a pointer to an array of `device_info_t` structures
 *         describing playback devices, or an error code and message on failure.
 */
FFI_PLUGIN_EXPORT result_t audio_context_get_playback_devices_info(const void *context);

/**
 * @brief Gets information about available capture devices.
 *
 * @param context Pointer to the Context.
 * @return A `result_t` structure containing a pointer to an array of `device_info_t` structures
 *         describing capture devices, or an error code and message on failure.
 */
FFI_PLUGIN_EXPORT result_t audio_context_get_capture_devices_info(const void *context);

/**
 * @brief Gets the number of bytes per sample for the given format.
 *
 * @param format Audio sample format.
 * @return The number of bytes per sample.
 */
FFI_PLUGIN_EXPORT uint32_t get_bytes_per_sample(sample_format_t format);

/**
 * @brief Gets the number of bytes per frame for the given format and channel count.
 *
 * @param format Audio sample format.
 * @param channels Number of audio channels.
 * @return The number of bytes per frame.
 */
FFI_PLUGIN_EXPORT uint32_t get_bytes_per_frame(sample_format_t format, uint32_t channels);

#endif  // AUDIO_CONTEXT_H
