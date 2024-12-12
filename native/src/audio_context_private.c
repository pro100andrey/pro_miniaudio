#include "../include/audio_context_private.h"

#include <stdlib.h>

#include "../include/logger.h"
#include "../include/miniaudio.h"

bool context_register_device(audio_context_t *self, audio_device_t *pDevice) {
    if (!self) {
        LOG_ERROR("invalid parameter: `self` is NULL.\n", "");
        return false;
    }

    if (!pDevice) {
        LOG_ERROR("invalid parameter: `pDevice` is NULL.\n", "");
        return false;
    }

    // Expand the pDevice list
    self->audioDevices = realloc(self->audioDevices,
                                 (self->deviceCount + 1) * sizeof(audio_device_t *));
    if (!self->audioDevices) {
        LOG_ERROR("Failed to allocate memory for audioDevices.\n", "");
        return false;
    }

    self->audioDevices[self->deviceCount++] = pDevice;

    LOG_INFO("<%p>(audio_device_t *) registered.\n", pDevice);

    return true;
}

bool context_unregister_device(audio_context_t *self, audio_device_t *pDevice) {
    if (!self) {
        LOG_ERROR("invalid parameter: `pContext` is NULL.\n", "");
        return false;
    }

    if (!pDevice) {
        LOG_ERROR("invalid parameter: `pDevice` is NULL.\n", "");
        return false;
    }

    bool found = false;

    // Find the pDevice in the list
    for (size_t i = 0; i < self->deviceCount; i++) {
        if (self->audioDevices[i] == pDevice) {
            found = true;

            // Shift remaining devices down
            for (size_t j = i; j < self->deviceCount - 1; j++) {
                self->audioDevices[j] = self->audioDevices[j + 1];
            }

            self->deviceCount--;
            break;
        }
    }

    if (!found) {
        LOG_ERROR("Device not found in context.\n", "");
        return false;
    }

    // Shrink the pDevice list
    self->audioDevices = realloc(self->audioDevices,
                                 self->deviceCount * sizeof(audio_device_t *));

    LOG_INFO("<%p>(audio_device_t *) unregistered.\n", pDevice);
    return true;
}
