#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdbool.h>
#include <time.h>

// Console color codes for output
#define GREEN_ESC_CODE_STR "\033[32m"
#define RED_ESC_CODE_STR   "\033[31m"
#define RESET_ESC_CODE_STR "\033[0m"

typedef enum COLOUR {
    GREEN,
    RED
} ColourEnum;

void printf_colour(ColourEnum colour, const char* format, ...);
void display_test_result(const char* test_name, bool passed, unsigned int line_num);
void display_test_summary(unsigned int num_tests, unsigned int pass_count, clock_t start_time, clock_t end_time);
void shuffle_array(void* arr, size_t elem_size, size_t n);

#endif /* TEST_UTILS_H */