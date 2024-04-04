#ifndef TEST_UTILS
#define TEST_UTILS

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include <setjmp.h>

#define ERROR_FILE_NAME "error.log"
FILE* _error_log_fp;

#define BUFFER_LEN 200
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
    ASSERT_EQUALS(strcmp(str1, str2), 0)

#define ASSERT_PRODUCED_ERROR_MSG(msg) \
    do { \
        ASSERT_NOT_NULL(fgets(BUFFER, BUFFER_LEN, _error_log_fp)); \
        ASSERT_STR_EQUAL(BUFFER, msg); \
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

// Used to mock 
static jmp_buf jump_buffer;

// Mock the assert macro
#define assert(condition) \
    if (!(condition)) { \
        longjmp(jump_buffer, 1); \
    }

// Mock malloc, calloc, assert 
#define malloc(x) (MALLOC_FAIL ? NULL : malloc(x))
#define calloc(x, y) (CALLOC_FAIL ? NULL : calloc(x, y))

// Consitent error messages across systems
#define strerror(errno) "ERROR"

bool MALLOC_FAIL = false; 
bool CALLOC_FAIL = false;

// // Placeholder definitions for BUFFER and BUFFER_LEN
// #define BUFFER_LEN 256
// char BUFFER[BUFFER_LEN];

#endif /* TEST_UTILS */
