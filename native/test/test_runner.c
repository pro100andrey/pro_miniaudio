#include "../include/context.h"
#include "../include/error_code.h"
#include "../include/playback_device.h"
#include "../include/waveform.h"
#include "../include/logger.h"
#include "unity/unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_context_create_destroy(void) {
    set_log_to_console_enabled(true);

    void *pContext = context_create();

    TEST_ASSERT_NOT_NULL(pContext);

    TEST_ASSERT_TRUE(context_is_valid(pContext));

    context_destroy(pContext);
}
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_context_create_destroy);

    return UNITY_END();
}
