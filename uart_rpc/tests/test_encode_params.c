#include "unity.h"
#include "rpc_common.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_simple(void) {
    // more test stuff
    char params[50];
    int val = 51;
    double db_val = -1.256;
    char str[50] = "hello";
    encode_params(params, 0, "%d%lf%s", val, db_val, str);
    TEST_ASSERT_EQUAL_STRING("d51,lf-1.256,shello", params);
    
    strcpy(str, "d2561lfa");
    val = -7113;
    encode_params(params, 0, "%s%d%lf", str, val, db_val);
    TEST_ASSERT_EQUAL_STRING("sd2561lfa,d-7113,lf-1.256", params);
}

void test_failure(void) {
    char params[50];
    encode_params(params, 0, "");
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_simple);
    RUN_TEST(test_failure);
    return UNITY_END();
}
