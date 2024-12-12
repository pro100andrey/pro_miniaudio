#include "../include/context_private.h"

#include <stdlib.h>

#include "../include/logger.h"
#include "../include/miniaudio.h"

bool context_register_device(context_t *pContext, audio_device_t *pDevice) {
    if (!pContext) {
        LOG_ERROR("invalid parameter: `pContext` is NULL.\n", "");
        return false;
    }

    if (!pDevice) {
        LOG_ERROR("invalid parameter: `pDevice` is NULL.\n", "");
        return false;
    }

    // Expand the pDevice list
    pContext->audioDevices = realloc(pContext->audioDevices,
                                     (pContext->deviceCount + 1) * sizeof(audio_device_t *));
    if (!pContext->audioDevices) {
        LOG_ERROR("Failed to allocate memory for audioDevices.\n", "");
        return false;
    }

    pContext->audioDevices[pContext->deviceCount++] = pDevice;

    LOG_INFO("<%p>(audio_device_t *) registered.\n", pDevice);

    return true;
}

bool context_unregister_device(context_t *pContext, audio_device_t *pDevice) {
    if (!pContext) {
        LOG_ERROR("invalid parameter: `pContext` is NULL.\n", "");
        return false;
    }

    if (!pDevice) {
        LOG_ERROR("invalid parameter: `pDevice` is NULL.\n", "");
        return false;
    }

    bool found = false;

    // Find the pDevice in the list
    for (size_t i = 0; i < pContext->deviceCount; i++) {
        if (pContext->audioDevices[i] == pDevice) {
            found = true;

            // Shift remaining devices down
            for (size_t j = i; j < pContext->deviceCount - 1; j++) {
                pContext->audioDevices[j] = pContext->audioDevices[j + 1];
            }

            pContext->deviceCount--;
            break;
        }
    }

    if (!found) {
        LOG_ERROR("Device not found in context.\n", "");
        return false;
    }

    // Shrink the pDevice list
    pContext->audioDevices = realloc(pContext->audioDevices,
                                     pContext->deviceCount * sizeof(audio_device_t *));

    LOG_INFO("<%p>(audio_device_t *) unregistered.\n", pDevice);
    return true;
}
