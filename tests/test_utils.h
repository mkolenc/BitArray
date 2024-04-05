#ifndef TEST_UTILS
#define TEST_UTILS

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <fcntl.h>
#include <unistd.h>


#define ERROR_FILE_NAME "error.log"
FILE* _error_log_fp;
fpos_t _error_log_position;

#define BUFFER_LEN 1024
char BUFFER[BUFFER_LEN] = {0};

// Macros for testing
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        display_results_(false, __func__, __LINE__); \
        return; \
    }

#define ASSERT_FALSE(condition) \
    ASSERT_TRUE(!(condition))

#define ASSERT_NULL(condition) \
    ASSERT_TRUE((condition) == NULL)

#define ASSERT_NOT_NULL(condition) \
    ASSERT_TRUE((condition) != NULL)

#define ASSERT_EQUALS(x, y) \
    ASSERT_TRUE((x) == (y))

#define ASSERT_STR_EQUAL(str1, str2) \
    ASSERT_TRUE(strcmp(str1, str2) == 0)

 #define CAPTURE_ERRORS(buffer, ...) \
        do { \
            fgetpos(_error_log_fp, &_error_log_position); \
            __VA_ARGS__; \
            fflush(stderr); \
            fsetpos(_error_log_fp, &_error_log_position); \
            fgets(buffer, BUFFER_LEN, _error_log_fp); \
        } while (0)

#define ASSERT_CRASH(code) \
    do { \
        int jump_code = setjmp(jump_buffer); \
        if (jump_code == 0) { \
            code; \
            ASSERT_TRUE(false); \
        } \
    } while (0)

// Update and display test status
int _TEST_COUNT = 0;
int _TEST_PASS_COUNT = 0;

void display_results_(bool passed, const char* test_name, int line_num)
{
    const int right_align_width = 50;
    printf("%s: ", test_name);
    for (int i = 0; i < (right_align_width - strlen(test_name)); ++i)
        putchar('-');

    _TEST_COUNT++;
    if (passed) {
        puts(" PASSED");
        _TEST_PASS_COUNT++;
    } else
        printf(" FAILED on line %d\n", line_num);
}

// Used for Mocking assert
static jmp_buf jump_buffer;

#define assert(condition) \
    if (!(condition)) { \
        longjmp(jump_buffer, 1); \
    }

#define exit(error_code) longjmp(jump_buffer, 1)


#define malloc(x) (MALLOC_FAIL ? NULL : malloc(x))
#define calloc(x, y) (CALLOC_FAIL ? NULL : calloc(x, y))
#define realloc(x, y) (REALLOC_FAIL ? NULL : realloc((x), (y)))

bool MALLOC_FAIL = false; 
bool CALLOC_FAIL = false;
bool REALLOC_FAIL = false;

// Consitent error messages across systems
#define strerror(errno) "ERROR"

#endif /* TEST_UTILS */
