#ifndef ERROR_CODE_H
#define ERROR_CODE_H

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
 * @brief Gets the description of an error code.
 *
 * @param code Error code to get the description for.
 * @return Description of the error code.
 */
const char* error_code_description(error_code_t code);

#endif  // ERROR_CODE_H
