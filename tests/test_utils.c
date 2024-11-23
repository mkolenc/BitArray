#include "test_utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// Colored printf for test results
void printf_colour(ColourEnum colour, const char* format, ...)
{
    if (colour == GREEN)
        printf(GREEN_ESC_CODE_STR);
    else
        printf(RED_ESC_CODE_STR);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf(RESET_ESC_CODE_STR);
}

// Display test result with color
void display_test_result(const char* test_name, bool passed, unsigned int line_num)
{
    const size_t right_align_width = 50;
    printf("%s: ", test_name);
    for (size_t i = 0; i < (right_align_width - strlen(test_name)); ++i)
        putchar('-');

    if (passed)
        printf_colour(GREEN, " passed\n");
    else
        printf_colour(RED, " FAILED on line %d\n", line_num);
}

// Display final summary of tests run
void display_test_summary(unsigned int num_tests, unsigned int pass_count, clock_t start_time, clock_t end_time)
{
    float elapsed_time_sec = (float)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("\n%u tests ran in %.3f seconds.\n", num_tests, elapsed_time_sec);

    unsigned int tests_failed = num_tests - pass_count;
    if (tests_failed)
        printf_colour(RED, "%d FAILED ", tests_failed);

    printf_colour(GREEN, "(%d tests passed)\n", pass_count);
}

// Shuffle array of test functions randomly
void shuffle_array(void* arr, size_t elem_size, size_t n)
{
    if (n <= 1)
        return;

    char* tmp = (char*) malloc(elem_size);
    if (tmp == NULL)
        exit(EXIT_FAILURE);

    srand(time(NULL));
    char* array = (char*) arr;

    for (size_t i = 0; i < n; ++i) {
        size_t j = rand() % n;
        memcpy(tmp, array + (j * elem_size), elem_size);
        memcpy(array + (j * elem_size), array + (i * elem_size), elem_size);
        memcpy(array + (i * elem_size), tmp, elem_size);
    }

    free(tmp);
}