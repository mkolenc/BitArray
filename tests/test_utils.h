#ifndef TEST_UTILS
#define TEST_UTILS

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define ERROR_FILE_NAME "error.log"
FILE* _error_log_fp;
fpos_t _error_log_position;
size_t _bytes_read;
bool _passed;
int _test_line;

#define BUFFER_LEN 1024
char BUFFER[BUFFER_LEN] = {0};

// Macros for testing
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        _passed = false; \
        _test_line = __LINE__; \
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
            _bytes_read = fread(buffer, sizeof(char), BUFFER_LEN - 1, _error_log_fp); \
            BUFFER[_bytes_read] = '\0'; \
        } while (0)

#define ASSERT_CRASH(code) \
    do { \
        int jump_code = setjmp(jump_buffer); \
        if (jump_code == 0) { \
            code; \
            ASSERT_TRUE(false); \
        } \
    } while (0)

// Used for Mocking assert and exit
static jmp_buf jump_buffer;

#define assert(condition) \
    if (!(condition)) { \
        longjmp(jump_buffer, 1); \
    }

#define exit(error_code) longjmp(jump_buffer, 1)

// Mock memory allocators
#define malloc(x) (_MALLOC_FAIL ? NULL : malloc(x))
#define calloc(x, y) (_CALLOC_FAIL ? NULL : calloc(x, y))
#define realloc(x, y) (_REALLOC_FAIL ? NULL : realloc((x), (y)))

bool _MALLOC_FAIL = false;
bool _CALLOC_FAIL = false;
bool _REALLOC_FAIL = false;

// Consistent error messages across systems
#define strerror(errno) "ERROR"

// Update and display test status
int _TEST_COUNT = 0;
int _TEST_PASS_COUNT = 0;

void display_results(bool _passed, const char* test_name, int line_num)
{
    const size_t right_align_width = 50;
    printf("%s: ", test_name);
    for (size_t i = 0; i < (right_align_width - strlen(test_name)); ++i)
        putchar('-');

    _TEST_COUNT++;
    if (_passed) {
        puts(" PASSED");
        _TEST_PASS_COUNT++;
    } else
        printf(" FAILED on line %d\n", line_num);
}

// For running tests in random order
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

#endif /* TEST_UTILS */
