#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdbool.h>

#include "constants.h"
#include "platform.h"

/**
 * @enum pcm_format_t
 * @brief Enumeration to represent common audio sample formats.
 *
 * Provides identifiers for common PCM (Pulse Code Modulation) audio formats
 * supported by the system. Each format specifies the type of data used
 * for individual audio samples.
 */
typedef enum {
    pcm_format_unknown = 0, /**< Unknown or unsupported PCM format. */
    pcm_format_u8 = 1,      /**< Unsigned 8-bit integer. */
    pcm_format_s16 = 2,     /**< Signed 16-bit integer. */
    pcm_format_s24 = 3,     /**< Signed 24-bit integer. */
    pcm_format_s32 = 4,     /**< Signed 32-bit integer. */
    pcm_format_f32 = 5,     /**< 32-bit floating point. */
    pcm_format_count        /**< Total number of supported PCM formats. */
} pcm_format_t;

/**
 * @struct audio_format_t
 * @brief Describes an audio data format.
 *
 * This structure contains details about the format of audio data, including
 * the PCM format, the number of channels, and the sample rate.
 */
typedef struct {
    pcm_format_t pcmFormat; /**< The PCM sample format (e.g., 16-bit integer). */
    uint32_t channels;      /**< The number of audio channels (e.g., 1 for mono, 2 for stereo). */
    uint32_t sampleRate;    /**< The sample rate in Hertz (e.g., 44100 Hz). */
} audio_format_t;

/**
 * @struct device_info_t
 * @brief Provides detailed information about an audio device.
 *
 * This structure describes an audio device, including its unique identifier,
 * its name, whether it is the default device, and the formats it supports.
 */
typedef struct {
    void *id;                              /**< Unique identifier for the device. */
    char name[MAX_DEVICE_NAME_LENGTH + 1]; /**< Null-terminated string representing the device name. */
    bool isDefault;                        /**< Indicates whether this is the default device. */
    uint32_t formatCount;                  /**< Number of supported audio formats. */
    audio_format_t audioFormats[64];       /**< Array of supported audio formats. */
} device_info_t;

/**
 * @brief Creates a new audio context.
 *
 * Initializes the audio system and prepares it for managing audio devices.
 *
 * @return A pointer to the newly created context, or NULL if the creation failed.
 */
FFI_PLUGIN_EXPORT
void *context_create(void);

/**
 * @brief Destroys an audio context.
 *
 * Frees all resources associated with the audio context, including
 * registered devices and cached device information.
 *
 * @param pContext Pointer to the context to destroy.
 */
FFI_PLUGIN_EXPORT
void context_destroy(void *pContext);

/**
 * @brief Refreshes the list of available audio devices.
 *
 * Scans the system for playback and capture devices, updating the context's
 * cached list of devices.
 *
 * @param pContext Pointer to the context.
 */
FFI_PLUGIN_EXPORT
void context_refresh_devices(const void *pContext);

/**
 * @brief Retrieves the number of playback devices.
 *
 * @param pContext Pointer to the context.
 * @return The total number of playback devices currently detected.
 */
FFI_PLUGIN_EXPORT
uint32_t context_get_playback_device_count(const void *pContext);

/**
 * @brief Retrieves the number of capture devices.
 *
 * @param pContext Pointer to the context.
 * @return The total number of capture devices currently detected.
 */
FFI_PLUGIN_EXPORT
uint32_t context_get_capture_device_count(const void *pContext);

/**
 * @brief Gets detailed information about playback devices.
 *
 * This function provides a pointer to an array of `device_info_t` structures,
 * each describing a detected playback device.
 *
 * @param pContext Pointer to the context.
 * @return A pointer to an array of `device_info_t` structures for playback devices.
 */
FFI_PLUGIN_EXPORT
device_info_t *context_get_playback_device_infos(const void *pContext);

/**
 * @brief Gets detailed information about capture devices.
 *
 * This function provides a pointer to an array of `device_info_t` structures,
 * each describing a detected capture device.
 *
 * @param pContext Pointer to the context.
 * @return A pointer to an array of `device_info_t` structures for capture devices.
 */
FFI_PLUGIN_EXPORT
device_info_t *context_get_capture_device_infos(const void *pContext);

#endif  // CONTEXT_H
