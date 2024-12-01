
#include "../include/result.h"

#include <stdarg.h>
#include <stdio.h>

#include "../include/logger.h"

result_t result_int(int32_t value) {
    return (result_t){
        .data.intData = value,
        .type = data_type_int,
    };
}

result_t result_ptr(void *value) {
    return (result_t){
        .data.pData = value,
        .type = data_type_ptr,
    };
}

result_t result_error(error_code_t code, const char *format, ...) {
    va_list args;
    va_start(args, format);

    result_t result = {
        .type = data_type_empty,
        .code = code,
    };

    vsnprintf(result.message, sizeof(result.message) - 1, format, args);
    result.message[sizeof(result.message) - 1] = '\0';

    va_end(args);

    LOG_ERROR("code %d, message: %s", code, result.message);

    return result;
}
