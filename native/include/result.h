#ifndef RESULT_H
#define RESULT_H

#include <stdint.h>

#include "error_code.h"

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
 * @brief Creates a result structure with an integer value.
 * @param value Integer value to store in the result.
 * @return A `result_t` structure containing the integer value.
 */
result_t result_int(int32_t value);

/**
 * @brief Creates a result structure with a pointer value.
 * @param value Pointer value to store in the result.
 * @return A `result_t` structure containing the pointer value.
 */
result_t result_ptr(void *value);

/**
 * @brief Creates a result structure with an error code and message.
 * @param errorCode Error code to store in the result.
 * @param format Format string for the error message.
 * @return A `result_t` structure containing the error code and message.
 */
result_t result_error(error_code_t code, const char *format, ...);

#endif  // RESULT_H
