#ifndef INTERNAL_H
#define INTERNAL_H

#include "miniaudio.h"

/**
 * @brief Returns a string representation of a `ma_format` value.
 *
 * @param format The `ma_format` to describe.
 * @return A string describing the `ma_format`, or "unknown" for invalid values.
 */
const char *describe_ma_format(ma_format format);

#endif  // INTERNAL_H
