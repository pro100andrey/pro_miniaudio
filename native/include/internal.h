#ifndef INTERNAL_H
#define INTERNAL_H

#include "miniaudio.h"

static const char *describe_ma_format(ma_format format) {
    switch (format) {
        case ma_format_unknown:
            return "unknown";
        case ma_format_u8:
            return "u8";
        case ma_format_s16:
            return "s16";
        case ma_format_s24:
            return "s24";
        case ma_format_s32:
            return "s32";
        case ma_format_f32:
            return "f32";
        default:
            return "unknown";
    }
}

#endif // !INTERNAL_H
