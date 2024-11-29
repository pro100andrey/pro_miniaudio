#include "pro_miniaudio.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_audio_context_create_destroy(void) {
    result_t result = audio_context_create();
    TEST_ASSERT_EQUAL_INT(error_code_none, result.code);
    TEST_ASSERT_NOT_NULL(result.data.pData);

    void *context = result.data.pData;
    TEST_ASSERT_TRUE(audio_context_is_valid(context));

    result_t destroy_result = audio_context_destroy(context);
    TEST_ASSERT_EQUAL_INT(error_code_none, destroy_result.code);
}

void test_audio_context_double_destroy(void) {
    result_t result = audio_context_create();
    TEST_ASSERT_EQUAL_INT(error_code_none, result.code);
    TEST_ASSERT_NOT_NULL(result.data.pData);

    void *context = result.data.pData;

    result_t destroy_result = audio_context_destroy(context);
    TEST_ASSERT_EQUAL_INT(error_code_none, destroy_result.code);
    context = NULL;
    destroy_result = audio_context_destroy(context);
    TEST_ASSERT_EQUAL_INT(error_code_context, destroy_result.code);
    TEST_ASSERT_EQUAL_STRING("Invalid audio context", destroy_result.message);
}

void test_resource_manager_free_all(void) {
    result_t result = audio_context_create();
    TEST_ASSERT_EQUAL_INT(error_code_none, result.code);

    resource_manager_free_all();
}

void test_audio_context_refresh_devices(void) {
    result_t result = audio_context_create();
    TEST_ASSERT_EQUAL_INT(error_code_none, result.code);
    TEST_ASSERT_NOT_NULL(result.data.pData);

    void *context = result.data.pData;

    result_t refresh_result = audio_context_refresh_devices(context);
    TEST_ASSERT_EQUAL_INT(error_code_none, refresh_result.code);

    result_t playback_device_count = audio_context_get_playback_device_count(context);
    TEST_ASSERT_TRUE(playback_device_count.code == error_code_none || playback_device_count.code == error_code_get_device);

    result_t capture_device_count = audio_context_get_capture_device_count(context);
    TEST_ASSERT_TRUE(capture_device_count.code == error_code_none || capture_device_count.code == error_code_get_device);

    audio_context_destroy(context);
}

void test_playback_device_create_and_destroy(void) {
    result_t context_result = audio_context_create();
    TEST_ASSERT_EQUAL_INT(error_code_none, context_result.code);
    TEST_ASSERT_NOT_NULL(context_result.data.pData);

    void *audio_context = context_result.data.pData;

    device_id_t device_id = {0};
    supported_format_t format = {0};

    result_t playback_device_result =
        playback_device_create(audio_context,
                               4096,
                               device_id,
                               format);

    TEST_ASSERT_EQUAL_INT(error_code_device, playback_device_result.code);

    if (playback_device_result.code == error_code_none) {
        void *playback_device = playback_device_result.data.pData;

        result_t start_result = playback_device_start(playback_device);
        TEST_ASSERT_EQUAL_INT(error_code_none, start_result.code);

        result_t stop_result = playback_device_stop(playback_device);
        TEST_ASSERT_EQUAL_INT(error_code_none, stop_result.code);

        result_t destroy_result = playback_device_destroy(playback_device);
        TEST_ASSERT_EQUAL_INT(error_code_none, destroy_result.code);
    }

    audio_context_destroy(audio_context);
}

void test_waveform_create_and_destroy(void) {
    result_t waveform_result =
        waveform_create(sample_format_f32,
                        2,
                        44100,
                        waveform_type_sine,
                        0.5,
                        440.0);

    TEST_ASSERT_EQUAL_INT(error_code_none, waveform_result.code);
    TEST_ASSERT_NOT_NULL(waveform_result.data.pData);

    void *waveform = waveform_result.data.pData;

    uint64_t frames_read = 0;
    float pcm_frames[2000];

    result_t read_result = waveform_read_pcm_frames_with_buffer(
        waveform,
        pcm_frames,
        1000,
        &frames_read);

    TEST_ASSERT_EQUAL_INT(error_code_none, read_result.code);
    TEST_ASSERT_EQUAL_UINT64(1000, frames_read);

    waveform_destroy(waveform);
}

void test_resource_manager_with_multiple_contexts(void) {
    result_t result1 = audio_context_create();
    TEST_ASSERT_EQUAL_INT(error_code_none, result1.code);

    result_t result2 = audio_context_create();
    TEST_ASSERT_EQUAL_INT(error_code_none, result2.code);

    result_t result3 = waveform_create(
        sample_format_f32,
        2,
        44100,
        waveform_type_sine,
        0.5,
        440.0);

    TEST_ASSERT_EQUAL_INT(error_code_none, result3.code);

    resource_manager_free_all();
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_audio_context_create_destroy);
    RUN_TEST(test_audio_context_double_destroy);
    RUN_TEST(test_resource_manager_free_all);
    RUN_TEST(test_audio_context_refresh_devices);
    RUN_TEST(test_playback_device_create_and_destroy);
    RUN_TEST(test_waveform_create_and_destroy);

    RUN_TEST(test_resource_manager_with_multiple_contexts);

    return UNITY_END();
}
