#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdbool.h>

#include "platform.h"

/**
 * union device_id_t
 * @brief Union for storing device identifiers for different audio backends.
 *
 * Each audio backend uses a specific type of identifier, tailored to its API requirements.
 * For example, WASAPI uses a wchar_t string, while DirectSound uses a GUID.
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
 * @brief Enumeration to represent supported audio sample formats.
 *
 * This enum provides identifiers for common audio sample formats supported by the system.
 */
typedef enum {
    sample_format_unknown = 0, /* Unknown or unsupported format. */
    sample_format_u8 = 1,      /* Unsigned 8-bit integer. */
    sample_format_s16 = 2,     /* Signed 16-bit integer. */
    sample_format_s24 = 3,     /* Signed 24-bit integer. */
    sample_format_s32 = 4,     /* Signed 32-bit integer. */
    sample_format_f32 = 5,     /* 32-bit floating point. */
    sample_format_count        /* Total number of supported formats. */
} sample_format_t;

/**
 * @struct supported_format_t
 * @brief Structure to describe a supported audio data format.
 *
 * Provides details about the sample format, number of channels, sample rate, and additional flags.
 */
typedef struct {
    sample_format_t format; /* Audio sample format. */
    uint32_t channels;      /* Number of audio channels. */
    uint32_t sampleRate;    /* Sample rate in Hertz. */
    uint32_t flags;         /* Format flags (reserved for future use). */
} supported_format_t;

/**
 * @def MAX_DEVICE_NAME_LENGTH
 * @brief Defines the maximum length of a device name, including the null terminator.
 */
#define MAX_DEVICE_NAME_LENGTH 255

/**
 * @struct device_info_t
 * @brief Structure to describe an audio device.
 *
 * Contains information about the device, including its identifier, name, whether it is the default device,
 * and the supported audio formats.
 */
typedef struct {
    device_id_t id;                        /* Unique identifier for the device. */
    char name[MAX_DEVICE_NAME_LENGTH + 1]; /* Null-terminated string representing the device name. */
    bool isDefault;                        /* Indicates if this is the default device. */
    uint32_t dataFormatCount;              /* Number of supported audio formats. */
    supported_format_t dataFormats[64];    /* Array of supported audio formats. */
} device_info_t;

/**
 * @brief Creates a new audio context.
 *
 * @return A pointer to the created context, or NULL if the creation failed.
 */
FFI_PLUGIN_EXPORT
void *context_create(void);

/**
 * @brief Destroys an audio context.
 *
 * Frees all associated resources.
 *
 * @param pContext Pointer to the context to destroy.
 */
FFI_PLUGIN_EXPORT
void context_destroy(void *pContext);

/**
 * @brief Refreshes the list of available audio devices.
 *
 * Updates both playback and capture device lists in the context.
 *
 * @param pContext Pointer to the context.
 */
FFI_PLUGIN_EXPORT
void context_refresh_devices(const void *pContext);

/**
 * @brief Gets the number of playback devices.
 *
 * @param pContext Pointer to the context.
 * @return The number of playback devices.
 */
FFI_PLUGIN_EXPORT
uint32_t context_get_playback_device_count(const void *pContext);

/**
 * @brief Gets the number of capture devices.
 *
 * @param pContext Pointer to the context.
 * @return The number of capture devices.
 */
FFI_PLUGIN_EXPORT
uint32_t context_get_capture_device_count(const void *pContext);

/**
 * @brief Retrieves playback device information.
 *
 * @param pContext Pointer to the context.
 * @return A pointer to an array of device_info_t structures.
 */
FFI_PLUGIN_EXPORT
device_info_t *context_get_playback_device_infos(const void *pContext);

/**
 * @brief Retrieves capture device information.
 *
 * @param pContext Pointer to the context.
 * @return A pointer to an array of device_info_t structures.
 */
FFI_PLUGIN_EXPORT
device_info_t *context_get_capture_device_infos(const void *pContext);

/**
 * @brief Gets the number of bytes per sample for a given format.
 *
 * @param format The audio sample format.
 * @return The number of bytes per sample.
 */
FFI_PLUGIN_EXPORT
uint32_t get_bytes_per_sample(sample_format_t format);

/**
 * @brief Gets the number of bytes per audio frame.
 *
 * @param format The audio sample format.
 * @param channels The number of channels in the audio frame.
 * @return The number of bytes per frame.
 */
FFI_PLUGIN_EXPORT
uint32_t get_bytes_per_frame(sample_format_t format, uint32_t channels);

#endif  // CONTEXT_H
