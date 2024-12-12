#include "../include/context.h"
#include "../include/logger.h"
#include "../include/playback_device.h"
#include "../include/waveform.h"
#include "unity/unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_context_create_destroy(void) {
    set_log_to_console_enabled(true);

    void *pContext = context_create();

    TEST_ASSERT_NOT_NULL(pContext);

    context_destroy(pContext);
}
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_context_create_destroy);

    return UNITY_END();
}
