#ifndef AUDIO_CONTEXT_H
#define AUDIO_CONTEXT_H

#include <stdbool.h>

#include "audio_device.h"
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
    device_id id;                          /**< Unique identifier for the device. */
    char name[MAX_DEVICE_NAME_LENGTH + 1]; /**< Null-terminated string representing the device name. */
    bool isDefault;                        /**< Indicates whether this is the default device. */
} device_info_t;

typedef struct {
    audio_device_type_t type;
    device_info_t *list;
    uint32_t count;
} device_infos_t;

/**
 * @struct device_info_ext_t
 * @brief Provides extended information about an audio device.
 *
 * This structure extends the `device_info_t` structure by including an array
 * of supported audio formats and the number of formats supported by the device.
 */
typedef struct {
    audio_format_t *list; /**< Array of supported audio formats. */
    uint32_t count;       /**< Number of supported audio formats. */
} device_info_ext_t;

/**
 * @brief Creates a new audio context.
 *
 * Initializes the audio system and prepares it for managing audio devices.
 *
 * @return A pointer to the newly created audio context structure `audio_context_t` , or NULL if the creation failed.
 */
FFI_PLUGIN_EXPORT
void *audio_context_create(void);

/**
 * @brief Destroys an audio context.
 *
 * Frees all resources associated with the audio context, including
 * registered devices and cached device information.
 *
 * @param self Pointer to the `audio_context_t` structure.
 */
FFI_PLUGIN_EXPORT
void audio_context_destroy(void *self);

/**
 * @brief Refreshes the list of available audio devices.
 *
 * Scans the system for playback and capture devices, updating the context's
 * cached list of devices.
 *
 * @param self Pointer to the `audio_context_t` structure.
 */
FFI_PLUGIN_EXPORT
void audio_context_refresh_devices(const void *self);

/*
 * @brief Retrieves the list of audio devices of the specified type.
 *
 * Returns a list of audio devices of the specified type (playback or capture)
 * that are currently available on the system.
 *
 * @param self Pointer to the `audio_context_t` structure.
 * @param type The type of audio devices to retrieve (playback or capture).
 * @return A pointer to the device information structure, or NULL if an error occurred.
 */

FFI_PLUGIN_EXPORT
device_infos_t *audio_context_get_device_infos(const void *self,
                                               audio_device_type_t type);

/**
 * @brief Destroys a device information structure.
 *
 * Frees the memory allocated for the device information structure and its contents.
 *
 * @param pDeviceInfos Pointer to the device information structure to destroy.
 */
FFI_PLUGIN_EXPORT
void audio_context_device_infos_destroy(device_infos_t *pDeviceInfos);

/*
 * @brief Retrieves extended information about an audio device.
 *
 * Retrieves detailed information about an audio device, including the list of
 * supported audio formats and the number of formats supported by the device.
 *
 * @param self Pointer to the `audio_context_t` structure.
 * @param deviceId Pointer to the unique identifier of the device.
 * @return A pointer to the extended device information structure, or NULL if an error occurred.
 */
FFI_PLUGIN_EXPORT
device_info_ext_t *audio_context_get_device_info_ext(const void *self,
                                                     void *deviceId);

/**
 * @brief Destroys an extended device information structure.
 *
 * Frees the memory allocated for the extended device information structure and its contents.
 * @param pDeviceInfoExt Pointer to the extended device information structure to destroy.
 */
FFI_PLUGIN_EXPORT
void audio_context_device_info_ext_destroy(device_info_ext_t *pDeviceInfoExt);

#endif  // AUDIO_CONTEXT_H
