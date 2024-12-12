
#ifndef PLAYBACK_DEVICE_INTERNAL_H
#define PLAYBACK_DEVICE_INTERNAL_H

#include "miniaudio.h"
#include "playback_device.h"

typedef struct {
    ma_device device;
    ma_rb rb;
    bool isReadingEnabled;

    playback_config_t config;
} playback_device_t;

#endif  // !PLAYBACK_DEVICE_INTERNAL_H
