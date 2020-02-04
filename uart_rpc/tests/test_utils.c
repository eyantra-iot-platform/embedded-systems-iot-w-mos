#include "unity.h"
#include "include/utils.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_function_code_method(void) {
    TEST_ASSERT_TRUE(code_method("getValve") == 10);
    TEST_ASSERT_TRUE(code_method("setValve") == 11);
    TEST_ASSERT_TRUE(code_method("toggleValve") == 12);
    TEST_ASSERT_TRUE(code_method("getFlow") == 20);
    TEST_ASSERT_TRUE(code_method("setRed") == 30);
    TEST_ASSERT_TRUE(code_method("setGreen") == 31);
    TEST_ASSERT_TRUE(code_method("setBlue") == 32);
}

void test_function_encode_params(void) {
    //more test stuff
    char param[50];
    int val = 51;
    double db_val = -1.256;
    char str[10] = "hello";
    encode_params(param, "int", &val);
    TEST_ASSERT_EQUAL_STRING("i51", param);
    
    val = -7113;
    encode_params(param, "int", &val);
    TEST_ASSERT_EQUAL_STRING("i-7113", param);
    encode_params(param, "double", &db_val);
    TEST_ASSERT_EQUAL_STRING("d-1.256", param);
    encode_params(param, "string", str);
    TEST_ASSERT_EQUAL_STRING("shello", param);
}

void test_function_concat_params() {
    char result[100];
    char param1[50] = "i755";
    char param2[50] = "sabcaojws";
    char param3[50] = "d-15.275";

    concat_params(result, param1, param2);
    TEST_ASSERT_EQUAL_STRING("i755,sabcaojws", result);

    concat_params(result, result, param3);
    TEST_ASSERT_EQUAL_STRING("i755,sabcaojws,d-15.275", result);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_function_code_method);
    RUN_TEST(test_function_encode_params);
    return UNITY_END();
}