#include "unity.h"
#include "rpc_common.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_simple_parse_request(void) {
    //more test stuff
    char method;
    int req_id;
    char message[50], params[35];
    strcpy(message, "(45305d798,sHello,lf7.165");
    parse_rpc_request(&method, &req_id, params, message);

    TEST_ASSERT_EQUAL_CHAR('(', method);
    TEST_ASSERT_EQUAL_INT(45305, req_id);
    TEST_ASSERT_EQUAL_STRING("d798,sHello,lf7.165", params);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_simple_parse_request);
    return UNITY_END();
}
