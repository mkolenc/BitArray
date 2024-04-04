#define TESTING_ENV
#include "test_utils.h"
#define BIT_ARRAY_IMPLEMENTATION
#include "../bit_array.h"

void test_BitArray_init_success(void)
{
    index_t size = 0;
    BitArray* bit_array = BitArray_init(size);

    ASSERT_NOT_NULL(bit_array);
    ASSERT_EQUALS(bit_array->num_bits, size);
	ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), "");

	BitArray_free(bit_array);
	size = 10;
	bit_array = BitArray_init(size);

	ASSERT_NOT_NULL(bit_array);
    ASSERT_EQUALS(bit_array->num_bits, size);
	ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), "0000000000");

    BitArray_free(bit_array);
}

void test_BitArray_init_allocation_failure(void)
{
    index_t size = 26;

	MALLOC_FAIL = true;
    ASSERT_NULL(BitArray_init(size));
	ASSERT_PRODUCED_ERROR_MSG("Failed to allocate BitArray struct: ERROR\n");

	MALLOC_FAIL = false;
	CALLOC_FAIL = true;
	ASSERT_NULL(BitArray_init(size));
	ASSERT_PRODUCED_ERROR_MSG("Failed to allocate BitArray data of size 26: ERROR\n");
}

void test_BitArray_set(void)
{
	index_t size = 24;
	BitArray* bit_array = BitArray_init(size);

	// BVA for index bounds
	ASSERT_CRASH(BitArray_set_bit(bit_array, 24));
	BitArray_set_bit(bit_array, 23);
	ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), "000000000000000000000001");

	// Setting an already set bit shouldn't modify it
	BitArray_set_bit(bit_array, 23);
	ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), "000000000000000000000001");

	// Correctly indexes into first and last bye
	BitArray_set_bit(bit_array, 0);
	ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), "100000000000000000000001");

	// Check all bit offsets in a byte
	char str[25] = {0};
	for (size_t i = 8; i < 16; ++i) {
		BitArray_clear(bit_array);
		BitArray_set_bit(bit_array, i);
		memset(str, '0', size);
		str[i] = '1';

		ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), str);
	}
}

void test_BitArray_clear_bit(void)
{
	index_t size = 24;
	BitArray* bit_array = BitArray_init(size);
	BitArray_set(bit_array);

	// BVA for index bounds
	ASSERT_CRASH(BitArray_clear_bit(bit_array, 24));
	BitArray_clear_bit(bit_array, 23);
	ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), "111111111111111111111110");

	// Clearing an already cleared bit shouldn't modify it
	BitArray_clear_bit(bit_array, 23);
	ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), "111111111111111111111110");

	// Correctly indexes into first and last bye
	BitArray_clear_bit(bit_array, 0);
	ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), "011111111111111111111110");

	// Check all bit offsets in a byte
	char str[25] = {0};
	for (size_t i = 8; i < 16; ++i) {
		BitArray_set(bit_array);
		BitArray_clear_bit(bit_array, i);
		memset(str, '1', size);
		str[i] = '0';

		ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), str);
	}
}
void test_BitArray_toggle_bit(void)
{
	index_t size = 24;
	BitArray* bit_array = BitArray_init(size);

	// BVA for index bounds
	ASSERT_CRASH(BitArray_toggle_bit(bit_array, 24));
	BitArray_toggle_bit(bit_array, 23);
	ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), "000000000000000000000001");

	// Correctly indexes into first and last bye
	BitArray_toggle_bit(bit_array, 0);
	ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), "100000000000000000000001");

	// Toggle every bit offset on and off
	char str[25] = {0};
	for (size_t i = 8; i < 16; ++i) {
		BitArray_clear(bit_array);

		BitArray_toggle_bit(bit_array, i);
		memset(str, '0', size);
		str[i] = '1';
		ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), str);

		BitArray_toggle_bit(bit_array, i);
		memset(str, '0', size);
		ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), str);
	}
}

void test_BitArray_check_bit(void)
{
	index_t size = 24;
	BitArray* bit_array = BitArray_init(size);

	ASSERT_CRASH(BitArray_toggle_bit(bit_array, 24));

	ASSERT_FALSE(BitArray_check_bit(bit_array, 0));
	ASSERT_FALSE(BitArray_check_bit(bit_array, 23));

	BitArray_set_bits(bit_array, 2, 0, 23);
	ASSERT_TRUE(BitArray_check_bit(bit_array, 0));
	ASSERT_TRUE(BitArray_check_bit(bit_array, 23));

	// Toggle every bit offset on and off
	for (size_t i = 8; i < 16; ++i) {
		BitArray_clear(bit_array);

		ASSERT_FALSE(BitArray_check_bit(bit_array, i));
		BitArray_set_bit(bit_array, i);
		ASSERT_TRUE(BitArray_check_bit(bit_array, i));
	}	
}

/////////////////// end of tests ///////////////////
#undef assert
#include <assert.h>
#include <time.h>
#include <unistd.h>  // for dup, dup2, close

typedef void (*TestFunction)(void);

typedef struct {
    TestFunction function;
    const char* name;
} TestFunctionInfo;


void before_all_tests(void)
{	
	// Redirect stderr to ERROR_FILE_NAME
	FILE* error_log_fp = fopen(ERROR_FILE_NAME, "w");
	assert(error_log_fp != NULL);
	assert(dup2(fileno(error_log_fp), STDERR_FILENO) != -1);
	fclose(error_log_fp);

	_error_log_fp = fopen(ERROR_FILE_NAME, "r");
	assert(_error_log_fp != NULL);
}

void after_all_tests(void)
{
	// Restore stderr and delete error log for tests
	dup2(STDERR_FILENO, fileno(stderr));
	fclose(_error_log_fp);
	remove(ERROR_FILE_NAME);

	printf("Passed %d/%d tests\n", _TEST_PASS_COUNT, _TEST_COUNT);
}

void shuffle_array(TestFunctionInfo arr[], int n)
{
	if (n <= 1)
		return;

	srand(time(NULL));

	for (int i = 0; i < n; ++i) {
		int j = rand() % n;
		TestFunctionInfo tmp = arr[j];
        arr[j] = arr[i];
		arr[i] = tmp;
	}
}

void run_tests(void)
{
    TestFunctionInfo test_funcs[] = {
        {test_BitArray_init_success, "test_BitArray_init_success"},
        {test_BitArray_init_allocation_failure, "test_BitArray_init_allocation_failure"},
        {test_BitArray_set, "test_BitArray_set"}, 
		{test_BitArray_clear_bit, "test_BitArray_clear_bit"},
		{test_BitArray_toggle_bit, "test_BitArray_toggle_bit"},
		{test_BitArray_check_bit, "test_BitArray_check_bit"}
    };
	const int NUM_TESTS = sizeof(test_funcs) / sizeof(test_funcs[0]);

	// Run the test in a random order
	shuffle_array(test_funcs, NUM_TESTS);

	for (int i = 0; i < NUM_TESTS; ++i) {
		// before_each
		test_funcs[i].function();
		// after_each
		display_results_(true, test_funcs[i].name, __LINE__);
		MALLOC_FAIL = false;
		CALLOC_FAIL = false;
	}
}

// Driving test code
int main(void)
{
	before_all_tests();
	run_tests();
	after_all_tests();

    return 0;
}