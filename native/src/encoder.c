#include "../include/encoder.h"

#include <stdlib.h>

#include "../include/logger.h"
#include "../include/miniaudio.h"

FFI_PLUGIN_EXPORT
void* encoder_create(const char* path, encoder_config_t* pConfig) {
    if (!path) {
        LOG_ERROR("Path is NULL");
        return NULL;
    }

    ma_encoder_config encoderConfig =
        ma_encoder_config_init(ma_encoding_format_wav,
                               (ma_format)pConfig->pcmFormat,
                               pConfig->channels,
                               pConfig->sampleRate);

    encoderConfig.encodingFormat = ma_encoding_format_wav;

    ma_encoder* encoder = malloc(sizeof(ma_encoder));

    if (!encoder) {
        LOG_ERROR("Failed to allocate ma_encoder");
        return NULL;
    }

    ma_result initEncoderResult =
        ma_encoder_init_file(path,
                             &encoderConfig,
                             encoder);

    if (initEncoderResult != MA_SUCCESS) {
        LOG_ERROR("Failed to initialize ma_encoder");
        free(encoder);
        return NULL;
    }

    return encoder;
}

FFI_PLUGIN_EXPORT
void encoder_destroy(void* self) {
    if (!self) {
        LOG_ERROR("ma_encoder is NULL");
        return;
    }

    ma_encoder* encoder = (ma_encoder*)self;
    ma_encoder_uninit(encoder);

    free(encoder);
}
