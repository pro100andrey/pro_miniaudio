#include "../include/audio_device.h"

#include "../include/logger.h"
#include "../include/miniaudio.h"

static audio_device_vtable_t g_audio_device_vtable = {
    .start = audio_device_start,
    .stop = audio_device_stop,
    .destroy = audio_device_destroy,
    .get_state = audio_device_get_state};

void audio_device_create(audio_device_t *dev, void *id, void *owner) {
    dev->vtable = &g_audio_device_vtable;
    dev->id = id;
    dev->owner = owner;
}

void audio_device_start(void *self) {
    audio_device_t *dev = (audio_device_t *)self;
    LOG_INFO("Audio device start (base)\n", "");
}

void audio_device_stop(void *self) {
    audio_device_t *dev = (audio_device_t *)self;
    LOG_INFO("Audio device stop (base)\n", "");
}

void audio_device_destroy(void *self) {
    audio_device_t *dev = (audio_device_t *)self;
    LOG_INFO("Audio device destroy (base)\n", "");
}

device_state_t audio_device_get_state(void *self) {
    audio_device_t *dev = (audio_device_t *)self;
    LOG_INFO("Audio device get_state (base)\n", "");

    return device_state_uninitialized;
}

audio_device_vtable_t *audio_device_get_base_vtable(void) {
    return &g_audio_device_vtable;
}
