#include "ui_utils.h"

bool clamp_int_test(const unsigned long test_count, const int input_v, const int input_min_v,
        const int input_max_v, const int expected_v) {
    int v = input_v;
    ui_utils_clamp_int(&v, input_min_v, input_max_v);
    
    const bool ret = v == expected_v;
    printf(
            "(%2lu | %s) %2d in [%2d ; %2d] => %d\n", 
            test_count, (ret ? "OK" : "NO"),
            input_v, input_min_v, input_max_v, expected_v
    );

    return ret;
}

bool clamp_size_t_test(const unsigned long test_count, const size_t input_v, const size_t input_min_v,
        const size_t input_max_v, const size_t expected_v) {
    size_t v = input_v;
    ui_utils_clamp_size_t(&v, input_min_v, input_max_v);
    
    const bool ret = v == expected_v;
    printf(
            "(%2lu | %s) %2lu in [%2lu ; %2lu] => %lu\n", 
            test_count, (ret ? "OK" : "NO"),
            input_v, input_min_v, input_max_v, expected_v
    );

    return ret;
}

bool reset_input_test(const unsigned long test_count) {
    user_selection_t s = {
        .input = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'l', 'd', ' ', '!', '\0'},
        .input_length = 13,
    };

    ui_utils_reset_input_buffer(&s);
    
    const bool ret = (s.input[0] == 0 && s.input_length == 0);

printf(
            "(%2lu | %s) [\"Hello world !\" : 13] => [\"\" : 0]\n",
            test_count, (ret ? "OK" : "NO")
    );

    return ret;
}

int main() {
    int i = 0;
    int ret = 0;
    
    printf("============== ui_utils_test ================\n");

    printf("\n---------- clamp int ------------------------\n");
    ret += clamp_int_test(++i, 42, 0, 50, 42);
    ret += clamp_int_test(++i, 42, 0, 40, 40);
    ret += clamp_int_test(++i, 42, 45, 50, 45);
    ret += clamp_int_test(++i, 42, 45, 45, 45);
    ret += clamp_int_test(++i, 42, 1, 1, 1);
    ret += clamp_int_test(++i, 42, 0, 50, 42);
    ret += clamp_int_test(++i, 42, 50, -3, 42);

    printf("\n---------- clamp size_t ---------------------\n");
    ret += clamp_size_t_test(++i, 42, 0, 50, 42);
    ret += clamp_size_t_test(++i, 42, 0, 40, 40);
    ret += clamp_size_t_test(++i, 42, 45, 50, 45);
    ret += clamp_size_t_test(++i, 42, 45, 45, 45);
    ret += clamp_size_t_test(++i, 42, 1, 1, 1);
    ret += clamp_size_t_test(++i, 42, 0, 50, 42);
    
    printf("\n---------- reset input ----------------------\n");
    ret += reset_input_test(++i);

    printf("\n %d / %d tests passed. (%3.1f%%)\n\n", ret, i, 100.0f * (float)ret/i);
}
