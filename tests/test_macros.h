#ifndef TEST_MACROS
#define TEST_MACROS

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#define TESTING_OUTPUT_FILE_NAME ("testing_output_log.txt")
#define TEST_BUFFER_LEN (1024)

typedef struct {
    bool passed;
    unsigned int line_number;

    bool malloc_fail;
    bool calloc_fail;
    bool realloc_fail;

    FILE* output_fp;
    char buffer[TEST_BUFFER_LEN];

    jmp_buf jump_buffer;
} TestData;

// Global test object
static TestData test_data = {0};

// Macros for testing
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        test_data.passed = false; \
        test_data.line_number = __LINE__; \
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

// Captures output from fprintf, and saves it to the buffer
#define CAPTURE_OUTPUT(...) \
    do { \
        test_data.output_fp = fopen(TESTING_OUTPUT_FILE_NAME, "w+"); \
        __VA_ARGS__; \
        fflush(test_data.output_fp); \
        rewind(test_data.output_fp); \
        size_t bytes_read = fread(test_data.buffer, 1, TEST_BUFFER_LEN - 1, test_data.output_fp); \
        test_data.buffer[bytes_read] = '\0'; \
        fclose(test_data.output_fp); \
    } while (0)

#define fprintf(stream, ...) fprintf(test_data.output_fp, __VA_ARGS__)

// Consistent error messages across systems
#define strerror(errno) "ERROR"

// Used for Mocking assert and exit
#define ASSERT_CRASH(code) \
    do { \
        int jump_code = setjmp(test_data.jump_buffer); \
        if (jump_code == 0) { \
            code; \
            ASSERT_TRUE(false); \
        } \
    } while (0)

#define assert(condition) \
    if (!(condition)) { \
        longjmp(test_data.jump_buffer, 1); \
    }

#define exit(error_code) longjmp(test_data.jump_buffer, 1)

// Mock memory allocators
#define malloc(x) (test_data.malloc_fail ? NULL : malloc(x))
#define calloc(x, y) (test_data.calloc_fail ? NULL : calloc(x, y))
#define realloc(x, y) (test_data.realloc_fail ? NULL : realloc((x), (y)))

#endif /* TEST_MACROS */