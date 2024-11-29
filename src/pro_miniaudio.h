#ifndef PRO_MINIAUDIO_H
#define PRO_MINIAUDIO_H

#include <stdbool.h>

#include "platform.h"

/**
 * @brief Frees all resources registered in the resource manager.
 *
 * This function is designed to handle the cleanup of all resources managed by
 * the resource manager. It is particularly useful in scenarios such as:
 * - **Hot Reload:** During development (e.g., in Flutter), where application
 *   state needs to be reset and resources reallocated to avoid memory leaks
 *   or dangling pointers.
 * - **Application Shutdown:** Ensures proper cleanup of resources during
 *   application termination to prevent resource leaks.
 *
 * Internally, this function calls `resource_manager_clear`, which:
 * - Iterates through all currently registered resources.
 * - Executes the cleanup function for each registered resource.
 * - Frees the memory associated with the resource manager.
 *
 * @note After calling this function, previously registered resources become invalid.
 * Ensure resources are reinitialized if this function is used during a hot reload.
 */

FFI_PLUGIN_EXPORT
void resource_manager_free_all(void);

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
 * @def MAX_DEVICE_NAME_LENGTH
 * @brief Defines the maximum length of a device name, including the null terminator.
 */
#define MAX_DEVICE_NAME_LENGTH 255

/**
 * @enum error_code_t
 * @brief Error codes for function results.
 */
typedef enum {
    error_code_none = 0,            /* No error. */
    error_code_unknown = 1,         /* Unknown error. */
    error_code_context = 2,         /* Context error. */
    error_code_device = 3,          /* Device error. */
    error_code_get_device = 5,      /* Failed to retrieve devices. */
    error_code_get_device_info = 6, /* Failed to retrieve device information. */
    error_code_buffer = 7,          /* Buffer error. */
    error_waveform = 8              /* Waveform error. */
} error_code_t;

/**
 * @enum data_type_t
 * @brief Enum to represent the type of data stored in a `result_t`.
 */
typedef enum {
    data_type_empty = 0, /* No data. */
    data_type_int = 1,   /* Integer data. */
    data_type_ptr = 2    /* Pointer to data. */
} data_type_t;

/**
 * union result_data_t
 * @brief Union to store different types of result data.
 */
typedef union {
    void *pData;     /* Generic pointer to data. */
    int32_t intData; /* Integer data. */
    float floatData; /* Floating-point data. */
} result_data_t;

/**
 * struct result_t
 * @brief Structure to represent the result of a function call.
 */
typedef struct {
    result_data_t data; /* Result data. */
    data_type_t type;   /* Type of the result data. */
    error_code_t code;  /* Error code indicating the result status. */
    char message[256];  /* Error message, if applicable. */
} result_t;

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
FFI_PLUGIN_EXPORT result_t audio_context_destroy(void *context);

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

/* Playback Device */

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
 * @param pAudioContext Pointer to the audio context.
 * @param bufferSizeInBytes Size of the internal buffer in bytes.
 * @param deviceId Identifier of the device to be used.
 * @param supportedFormat Format configuration for the playback device.
 * @return A `result_t` structure containing a pointer to the playback device or an error code.
 */
FFI_PLUGIN_EXPORT result_t playback_device_create(void *pAudioContext,
                                                  size_t bufferSizeInBytes,
                                                  device_id_t deviceId,
                                                  supported_format_t supportedFormat);

/**
 * @brief Destroys a playback device and releases its resources.
 *
 * @param device Pointer to the playback device to destroy.
 * @return A `result_t` structure indicating success or failure.
 */
FFI_PLUGIN_EXPORT result_t playback_device_destroy(void *device);

/**
 * @brief Starts playback on the device.
 *
 * @param device Pointer to the playback device.
 * @return A `result_t` structure indicating success or failure.
 */
FFI_PLUGIN_EXPORT result_t playback_device_start(void *device);

/**
 * @brief Stops playback on the device.
 *
 * @param device Pointer to the playback device.
 * @return A `result_t` structure indicating success or failure.
 */
FFI_PLUGIN_EXPORT result_t playback_device_stop(void *device);

/**
 * @brief Pushes audio data to the playback device's buffer.
 *
 * @param device Pointer to the playback device.
 * @param data Pointer to the data to be pushed.
 * @return A `result_t` structure indicating success or failure.
 */
FFI_PLUGIN_EXPORT result_t playback_device_push_buffer(void *device, playback_data_t *data);

/* Waveform generation */

/**
 * @enum waveform_type_t
 * @brief Enum representing different types of waveforms.
 */
typedef enum {
    waveform_type_sine,     /* Sine waveform. */
    waveform_type_square,   /* Square waveform. */
    waveform_type_triangle, /* Triangle waveform. */
    waveform_type_sawtooth  /* Sawtooth waveform. */
} waveform_type_t;

/**
 * @brief Creates a waveform generator with the specified parameters.
 *
 * @param format Audio sample format.
 * @param channels Number of audio channels.
 * @param sampleRate Sample rate in Hertz.
 * @param waveformType Type of waveform to generate.
 * @param amplitude Amplitude of the waveform.
 * @param frequency Frequency of the waveform in Hertz.
 * @return A `result_t` structure containing a pointer to the waveform generator or an error code.
 */
FFI_PLUGIN_EXPORT result_t waveform_create(sample_format_t format,
                                           uint32_t channels,
                                           uint32_t sampleRate,
                                           waveform_type_t waveformType,
                                           double amplitude,
                                           double frequency);

/**
 * @brief Destroys a waveform generator and releases its resources.
 *
 * @param waveform Pointer to the waveform generator to destroy.
 */
FFI_PLUGIN_EXPORT void waveform_destroy(void *waveform);

/**
 * @brief Reads PCM frames from the waveform generator.
 *
 * @param waveform Pointer to the waveform generator.
 * @param pFramesOut Pointer to the output buffer for PCM frames.
 * @param framesCount Number of frames to read.
 * @param pFramesRead Pointer to a variable to store the number of frames read.
 * @return A `result_t` structure indicating success or failure.
 */
FFI_PLUGIN_EXPORT result_t waveform_read_pcm_frames_with_buffer(void *waveform,
                                                                void *pFramesOut,
                                                                uint64_t framesCount,
                                                                uint64_t *pFramesRead);
/* Utility Functions */

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

#endif /* PRO_MINIAUDIO_H */
