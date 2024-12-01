#include "error_code.h"

const char* error_code_description(error_code_t code) {
    switch (code) {
        case error_code_none:
            return "No error";
        case error_code_unknown:
            return "Unknown error";
        case error_code_context:
            return "Context error";
        case error_code_device:
            return "Device error";
        case error_code_get_device:
            return "Failed to retrieve devices";
        case error_code_get_device_info:
            return "Failed to retrieve device information";
        case error_code_buffer:
            return "Buffer error";
        case error_waveform:
            return "Waveform error";
        default:
            return "Unknown error";
    }
}
