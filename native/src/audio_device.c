#include "../include/audio_device.h"

#include <string.h>

#include "../include/logger.h"
#include "../include/miniaudio.h"

static audio_device_vtable_t g_audio_device_vtable = {
    .start = audio_device_start,
    .stop = audio_device_stop,
    .destroy = audio_device_destroy,
    .get_state = audio_device_get_state};

void audio_device_create(audio_device_t *dev, device_id *id, void *owner, audio_device_type_t type) {
    dev->vtable = &g_audio_device_vtable;

    if (id) {
        memcpy(&dev->id, id, sizeof(device_id));
    }

    dev->owner = owner;
    dev->type = type;
}

void audio_device_start(void *self) {
    audio_device_t *dev = (audio_device_t *)self;
    LOG_INFO("<%p>(audio_device_t *) start (base)\n", dev);
}

void audio_device_stop(void *self) {
    audio_device_t *dev = (audio_device_t *)self;
    LOG_INFO("<%p>(audio_device_t *) stop (base)\n", dev);
}

void audio_device_destroy(void *self) {
    audio_device_t *dev = (audio_device_t *)self;
    LOG_INFO("<%p>(audio_device_t *) destroy (base)\n", dev);
}

device_state_t audio_device_get_state(void *self) {
    audio_device_t *dev = (audio_device_t *)self;
    LOG_INFO("<%p>(audio_device_t *) get_state (base)\n", dev);

    return device_state_uninitialized;
}

audio_device_vtable_t *audio_device_get_base_vtable(void) {
    return &g_audio_device_vtable;
}
