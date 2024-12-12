#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdbool.h>

#include "common.h"
#include "platform.h"
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
 * @struct audio_format_t
 * @brief Structure to describe a audio data format.
 *
 * Provides details about the sample format, number of channels, sample rate, and additional flags.
 */
typedef struct {
    sample_format_t sampleFormat; /* Audio sample format. */
    uint32_t channels;            /* Number of audio channels. */
    uint32_t sampleRate;          /* Sample rate in Hertz. */
} audio_format_t;

/**
 * @struct device_info_t
 * @brief Structure to describe an audio device.
 *
 * Contains information about the device, including its identifier, name, whether it is the default device,
 * and the supported audio formats.
 */
typedef struct {
    void *id;                              /* Unique identifier for the device. */
    char name[MAX_DEVICE_NAME_LENGTH + 1]; /* Null-terminated string representing the device name. */
    bool isDefault;                        /* Indicates if this is the default device. */
    uint32_t formatCount;                  /* Number of audio formats. */
    audio_format_t audioFormats[64];       /* Array of audio formats. */
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

#endif  // CONTEXT_H
