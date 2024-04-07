#define TESTING_ENV
#include "test_utils.h"
#define BIT_ARRAY_IMPLEMENTATION
#include "../bit_array.h"

void test_BitArray_init(void)
{
    index_t size = 26;
    _MALLOC_FAIL = true;
    CAPTURE_ERRORS(BUFFER,
        ASSERT_NULL(BitArray_init(size));
    );
    ASSERT_STR_EQUAL(BUFFER, "Failed to allocate BitArray struct: ERROR\n");

    _MALLOC_FAIL = false;
    _CALLOC_FAIL = true;
    CAPTURE_ERRORS(BUFFER,
        ASSERT_NULL(BitArray_init(size));
    );
    ASSERT_STR_EQUAL(BUFFER, "Failed to allocate BitArray data of size 26: ERROR\n");
    _CALLOC_FAIL = false;

    size = 0;
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

void test_BitArray_set(void)
{
    index_t size = 24;
    BitArray* bit_array = BitArray_init(size);
    ASSERT_NOT_NULL(bit_array);

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

    BitArray_free(bit_array);
}

void test_BitArray_region_operations(void)
{
    index_t size = 24;
    BitArray* b = BitArray_init(size);

    // Test invalid bounds
    ASSERT_CRASH(BitArray_set_region(b, 23, 24));
    ASSERT_CRASH(BitArray_set_region(b, 24, 4));
    ASSERT_CRASH(BitArray_clear_region(b, 23, 24));
    ASSERT_CRASH(BitArray_clear_region(b, 24, 999));
    ASSERT_CRASH(BitArray_toggle_region(b, 23, 24));
    ASSERT_CRASH(BitArray_toggle_region(b, 24, 123));

    // Index's are in the same byte
    BitArray_set_region(b, 9, 11);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "000000000111000000000000");
    BitArray_toggle_region(b, 8, 13);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "000000001000110000000000");
    BitArray_clear_region(b, 9, 15);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "000000001000000000000000");

    // Startin index is a MSB, ending index is LSB
    BitArray_set_region(b, 8, 23);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "000000001111111111111111");
    BitArray_toggle_region(b, 0, 15);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "111111110000000011111111");
    BitArray_clear_region(b, 15, 23);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "111111110000000000000000");

    // Starting / ending index are in middle of byte
    BitArray_toggle_region(b, 4, 12);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "111100001111100000000000");
    BitArray_set_region(b, 6, 16);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "111100111111111110000000");
    BitArray_clear_region(b, 2, 9);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "110000000011111110000000");

    BitArray_free(b);
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

    BitArray_free(bit_array);
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

    BitArray_free(bit_array);
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

    BitArray_free(bit_array);
}

void test_BitArray_init_from_hex(void)
{
    // Bad wheather tests
    ASSERT_CRASH(BitArray_init_from_hex(NULL));

    _MALLOC_FAIL = true;
    CAPTURE_ERRORS(BUFFER,
        ASSERT_NULL(BitArray_init_from_hex("abd00xf"))
    );
    ASSERT_STR_EQUAL(BUFFER, "Failed to allocate BitArray struct: ERROR\n");

    _MALLOC_FAIL = false;
    CAPTURE_ERRORS(BUFFER,
        ASSERT_CRASH(BitArray_init_from_hex("adm0Xf"));
    );
    ASSERT_STR_EQUAL(BUFFER, "Invalid hex string\n");

    // good wheather tests
    BitArray* bit_array = BitArray_init_from_hex("");
    ASSERT_NOT_NULL(bit_array);
    ASSERT_TRUE(bit_array->num_bits == 0);
    ASSERT_STR_EQUAL(BitArray_to_hex_str(bit_array, BUFFER), "");

    bit_array = BitArray_init_from_hex("0123456789aBcDeF");
    ASSERT_NOT_NULL(bit_array);
    ASSERT_TRUE(bit_array->num_bits == 64);
    ASSERT_STR_EQUAL(BitArray_to_hex_str(bit_array, BUFFER), "0123456789ABCDEF");

    BitArray_free(bit_array);
}

void test_BitArray_init_from_bin(void)
{
    // Bad wheather tests
    ASSERT_CRASH(BitArray_init_from_bin(NULL));

    _MALLOC_FAIL = true;
    CAPTURE_ERRORS(BUFFER,
        ASSERT_NULL(BitArray_init_from_bin("010101"))
    );
    ASSERT_STR_EQUAL(BUFFER, "Failed to allocate BitArray struct: ERROR\n");

    _MALLOC_FAIL = false;
    CAPTURE_ERRORS(BUFFER,
        ASSERT_CRASH(BitArray_init_from_bin("0101ff"));
    );
    ASSERT_STR_EQUAL(BUFFER, "Invalid binary string.\n");

    // good wheather tests
    BitArray* bit_array = BitArray_init_from_bin("");
    ASSERT_NOT_NULL(bit_array);
    ASSERT_TRUE(bit_array->num_bits == 0);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), "");

    bit_array = BitArray_init_from_bin("00000010101010");
    ASSERT_NOT_NULL(bit_array);
    ASSERT_TRUE(bit_array->num_bits == 14);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, BUFFER), "00000010101010");

    BitArray_free(bit_array);
}

void test_BitArray_resize(void)
{
    index_t size = 64;
    BitArray* b = BitArray_init(size);
    ASSERT_NOT_NULL(b);

    ASSERT_NULL(BitArray_resize(b, 0));

    _REALLOC_FAIL = true;
    CAPTURE_ERRORS(BUFFER,
        ASSERT_NULL(BitArray_resize(b, 1902));
    );
    ASSERT_STR_EQUAL(BUFFER, "Unable to resize BitArray to size 1902 bits: ERROR\n");
    _REALLOC_FAIL = false;

    BitArray_set(b);

    // Resize to same size (should do nothing)
    ASSERT_EQUALS(BitArray_resize(b, size), b);
    ASSERT_EQUALS(b->num_bits, size);
    ASSERT_STR_EQUAL(BitArray_to_hex_str(b, BUFFER), "FFFFFFFFFFFFFFFF");

    // Decrease size
    size = 24;
    ASSERT_EQUALS(BitArray_resize(b, size), b);
    ASSERT_EQUALS(b->num_bits, size);
    ASSERT_STR_EQUAL(BitArray_to_hex_str(b, BUFFER), "FFFFFF");

    // Increase size, zeros should be added
    size = 64;
    ASSERT_EQUALS(BitArray_resize(b, size), b);
    ASSERT_EQUALS(b->num_bits, size);
    ASSERT_STR_EQUAL(BitArray_to_hex_str(b, BUFFER), "FFFFFF0000000000");

    BitArray_free(b);
}

void test_BitArray_copy(void)
{
    BitArray* b = BitArray_init_from_hex("AB2255657B7B756DAA083");
    ASSERT_NOT_NULL(b);

    _MALLOC_FAIL = true;
    CAPTURE_ERRORS(BUFFER,
        ASSERT_NULL(BitArray_copy(b));
    );
    _MALLOC_FAIL = false;

    BitArray* copy = BitArray_copy(b);
    ASSERT_STR_EQUAL(BitArray_to_hex_str(copy, BUFFER), "AB2255657B7B756DAA083");


    BitArray_free(copy);
    BitArray_free(b);
}

void test_BitArray_next_clear_bit(void)
{
    index_t size = 777;
    BitArray* b = BitArray_init(size);
    ASSERT_NOT_NULL(b);

    index_t result = 0;
    index_t pos_1 = 69;
    index_t pos_2 = 420;

    BitArray_set_bits(b, 2, pos_1, pos_2);
    BitArray_toggle(b);

    ASSERT_TRUE(BitArray_next_clear_bit(b, 0, &result));
    ASSERT_TRUE(result == pos_1);
    ASSERT_FALSE(BitArray_next_clear_bit(b, size - 1, &result));
    ASSERT_CRASH(BitArray_next_clear_bit(b, size, &result));

    ASSERT_TRUE(BitArray_next_clear_bit(b, pos_1 - 1, &result));
    ASSERT_TRUE(result == pos_1);

    ASSERT_TRUE(BitArray_next_clear_bit(b, pos_1, &result));
    ASSERT_TRUE(result == pos_1);

    ASSERT_TRUE(BitArray_next_clear_bit(b, pos_1 + 1, &result));
    ASSERT_TRUE(result == pos_2);

    BitArray_free(b);
}

void test_BitArray_modify_multiple_bits(void)
{
    index_t size = 10;
    BitArray* b = BitArray_init(size);
    ASSERT_NOT_NULL(b);

    BitArray_set_bits(b, 4, 0, 3, 5, 9);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "1001010001");

    BitArray_clear_bits(b, 2, 0, 9);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "0001010000");

    BitArray_toggle_bits(b, 5, 2, 3, 4, 5, 6);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "0010101000");

    BitArray_free(b);
}

void test_BitArray_print_hex(void)
{
    BitArray* b = BitArray_init_from_hex("0123456789ABCDEF");
    ASSERT_NOT_NULL(b);

    ASSERT_CRASH(BitArray_print_hex(b, stderr, 0));

    CAPTURE_ERRORS(BUFFER,
        BitArray_print_hex(b, stderr, 16);
    );
    ASSERT_STR_EQUAL(BUFFER, "0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F\n");

    CAPTURE_ERRORS(BUFFER,
        BitArray_print_hex(b, stderr, 15);
    );
    ASSERT_STR_EQUAL(BUFFER, "0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E\nF\n");

    CAPTURE_ERRORS(BUFFER,
        BitArray_print_hex(b, stderr, 3);
    );
    ASSERT_STR_EQUAL(BUFFER, "0, 1, 2\n3, 4, 5\n6, 7, 8\n9, A, B\nC, D, E\nF\n");

    BitArray_free(b);
}

void test_BitArray_file_save(void)
{
    // Load should fail on non-BitArray data
    const char* tmp = "some_text.txt";
    FILE* fp = fopen(tmp, "w");
    ASSERT_NOT_NULL(fp);
    fprintf(fp, "Shall I compare thee to a summers day?");
    fclose(fp);

    BitArray* b;
    CAPTURE_ERRORS(BUFFER,
        b = BitArray_load(tmp);
    );
    ASSERT_STR_EQUAL(BUFFER, "some_text.txt does not contain BitArray data.\n");

    const char* hex_str = "ADF3527FA0009382777D7A73625";
    b = BitArray_init_from_hex(hex_str);
    ASSERT_NOT_NULL(b);

    ASSERT_TRUE(BitArray_save(b, tmp));
    BitArray_free(b);

    BitArray* loaded = BitArray_load(tmp);
    ASSERT_NOT_NULL(loaded);

    ASSERT_STR_EQUAL(BitArray_to_hex_str(loaded, BUFFER), hex_str);

    BitArray_free(loaded);
    remove(tmp);
}

void test_BitArray_print_bin(void)
{
    BitArray* b = BitArray_init_from_bin("0100001010101011");
    ASSERT_NOT_NULL(b);

    ASSERT_CRASH(BitArray_print_bin(b, stderr, 0));

    CAPTURE_ERRORS(BUFFER,
        BitArray_print_bin(b, stderr, 16);
    );
    ASSERT_STR_EQUAL(BUFFER, "0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1\n");

    CAPTURE_ERRORS(BUFFER,
        BitArray_print_bin(b, stderr, 15);
    );
    ASSERT_STR_EQUAL(BUFFER, "0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1\n1\n");

    CAPTURE_ERRORS(BUFFER,
        BitArray_print_bin(b, stderr, 2);
    );
    ASSERT_STR_EQUAL(BUFFER, "0, 1\n0, 0\n0, 0\n1, 0\n1, 0\n1, 0\n1, 0\n1, 1\n");

    BitArray_free(b);
}

void test_BitArray_operations(void)
{
    index_t size = 20;
    BitArray* b = BitArray_init(size);
    ASSERT_NOT_NULL(b);

    BitArray_set(b);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "11111111111111111111");

    BitArray_clear(b);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "00000000000000000000");

    BitArray_set_bits(b, 3, 2, 12, 16);
    BitArray_toggle(b);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, BUFFER), "11011111111101110111");

    BitArray_free(b);
}

void test_BitArray_next_set_bit(void)
{
    index_t size = 1000;
    BitArray* b = BitArray_init(size);
    ASSERT_NOT_NULL(b);

    index_t result = 0;
    index_t pos_1 = 69;
    index_t pos_2 = 420;

    BitArray_set_bits(b, 2, pos_1, pos_2);

    ASSERT_TRUE(BitArray_next_set_bit(b, 0, &result));
    ASSERT_TRUE(result == pos_1);
    ASSERT_FALSE(BitArray_next_set_bit(b, size - 1, &result));
    ASSERT_CRASH(BitArray_next_set_bit(b, size, &result));

    ASSERT_TRUE(BitArray_next_set_bit(b, pos_1 - 1, &result));
    ASSERT_TRUE(result == pos_1);

    ASSERT_TRUE(BitArray_next_set_bit(b, pos_1, &result));
    ASSERT_TRUE(result == pos_1);

    ASSERT_TRUE(BitArray_next_set_bit(b, pos_1 + 1, &result));
    ASSERT_TRUE(result == pos_2);

    BitArray_free(b);
}

void test_BitArray_prev_set_bit(void)
{
    index_t size = 823;
    BitArray* b = BitArray_init(size);
    ASSERT_NOT_NULL(b);

    index_t result = 0;
    index_t pos_1 = 69;
    index_t pos_2 = 420;

    BitArray_set_bits(b, 2, pos_1, pos_2);

    ASSERT_FALSE(BitArray_prev_set_bit(b, 0, &result));
    ASSERT_TRUE(BitArray_prev_set_bit(b, size - 1, &result));
    ASSERT_TRUE(result == pos_2);
    ASSERT_CRASH(BitArray_prev_set_bit(b, size, &result));

    ASSERT_TRUE(BitArray_prev_set_bit(b, pos_2 + 1, &result));
    ASSERT_TRUE(result == pos_2);

    ASSERT_TRUE(BitArray_prev_set_bit(b, pos_2, &result));
    ASSERT_TRUE(result == pos_2);

    ASSERT_TRUE(BitArray_prev_set_bit(b, pos_2 - 1, &result));
    ASSERT_TRUE(result == pos_1);

    BitArray_free(b);
}

void test_BitArray_first_and_last_set(void)
{
    index_t size = 1000;
    BitArray* b = BitArray_init(size);
    ASSERT_NOT_NULL(b);

    index_t result = 0;
    index_t pos_1 = 69;
    index_t pos_2 = 420;

    ASSERT_FALSE(BitArray_first_set_bit(b, &result));
    ASSERT_FALSE(BitArray_last_set_bit(b, &result));

    BitArray_set_bits(b, 2, 0, size - 1);

    ASSERT_TRUE(BitArray_first_set_bit(b, &result));
    ASSERT_TRUE(result == 0);
    ASSERT_TRUE(BitArray_last_set_bit(b, &result));
    ASSERT_TRUE(result == (size - 1));

    BitArray_clear(b);
    BitArray_set_bits(b, 2, 5, 69);

    ASSERT_TRUE(BitArray_first_set_bit(b, &result));
    ASSERT_TRUE(result == 5);
    ASSERT_TRUE(BitArray_last_set_bit(b, &result));
    ASSERT_TRUE(result == 69);

    BitArray_free(b);
}

void test_BitArray_size(void)
{
    index_t size = 0;
    BitArray* b = BitArray_init(size);
    ASSERT_NOT_NULL(b);

    ASSERT_EQUALS(BitArray_size(b), size);

    size = 285;
    ASSERT_NOT_NULL(BitArray_resize(b, size));
    ASSERT_EQUALS(BitArray_size(b), size);

    BitArray_free(b);
}

void test_BitArray_prev_clear_bit(void)
{
    index_t size = 689;
    BitArray* b = BitArray_init(size);
    ASSERT_NOT_NULL(b);

    index_t result = 0;
    index_t pos_1 = 69;
    index_t pos_2 = 420;

    BitArray_set_bits(b, 2, pos_1, pos_2);
    BitArray_toggle(b);

    ASSERT_FALSE(BitArray_prev_clear_bit(b, 0, &result));
    ASSERT_TRUE(BitArray_prev_clear_bit(b, size - 1, &result));
    ASSERT_TRUE(result == pos_2);
    ASSERT_CRASH(BitArray_prev_clear_bit(b, size, &result));

    ASSERT_TRUE(BitArray_prev_clear_bit(b, pos_2 + 1, &result));
    ASSERT_TRUE(result == pos_2);

    ASSERT_TRUE(BitArray_prev_clear_bit(b, pos_2, &result));
    ASSERT_TRUE(result == pos_2);

    ASSERT_TRUE(BitArray_prev_clear_bit(b, pos_2 - 1, &result));
    ASSERT_TRUE(result == pos_1);

    BitArray_free(b);
}

void test_BitArray_to_strings(void)
{
    BitArray* b = BitArray_init_from_hex("");
    ASSERT_NOT_NULL(b);

    ASSERT_CRASH(BitArray_to_hex_str(b, NULL));

    ASSERT_NOT_NULL(BitArray_to_bin_str(b, BUFFER));
    ASSERT_STR_EQUAL(BUFFER, "");
    ASSERT_NOT_NULL(BitArray_to_hex_str(b, BUFFER));
    ASSERT_STR_EQUAL(BUFFER, "");

    BitArray_free(b);

    const char* bin_str = "0000000100100011010001010110011110001001101010111100110111101111";
    const char* hex_str = "0123456789ABCDEF";

    b = BitArray_init_from_hex(hex_str);
    ASSERT_NOT_NULL(b);

    ASSERT_NOT_NULL(BitArray_to_bin_str(b, BUFFER));
    ASSERT_STR_EQUAL(BUFFER, bin_str);
    ASSERT_NOT_NULL(BitArray_to_hex_str(b, BUFFER));
    ASSERT_STR_EQUAL(BUFFER, hex_str);

    BitArray_free(b);
}

void test_BitArray_min_str_lengths(void)
{
    BitArray* b = BitArray_init(0);
    ASSERT_NOT_NULL(b);
    ASSERT_TRUE(BitArray_min_hex_str_len(b) == 1);
    ASSERT_TRUE(BitArray_min_bin_str_len(b) == 1);

    ASSERT_NOT_NULL(BitArray_resize(b, 16));
    ASSERT_TRUE(BitArray_min_hex_str_len(b) == 5);
    ASSERT_TRUE(BitArray_min_bin_str_len(b) == 17);

    ASSERT_NOT_NULL(BitArray_resize(b, 17));
    ASSERT_TRUE(BitArray_min_hex_str_len(b) == 6);
    ASSERT_TRUE(BitArray_min_bin_str_len(b) == 18);

    BitArray_free(b);
}

void test_BitArray_first_and_last_clear(void)
{
    index_t size = 489;
    BitArray* b = BitArray_init(size);
    ASSERT_NOT_NULL(b);

    index_t result = 0;
    index_t pos_1 = 69;
    index_t pos_2 = 420;

    BitArray_set(b);

    ASSERT_FALSE(BitArray_first_clear_bit(b, &result));
    ASSERT_FALSE(BitArray_last_clear_bit(b, &result));

    BitArray_clear_bits(b, 2, 0, size - 1);

    ASSERT_TRUE(BitArray_first_clear_bit(b, &result));
    ASSERT_TRUE(result == 0);
    ASSERT_TRUE(BitArray_last_clear_bit(b, &result));
    ASSERT_TRUE(result == (size - 1));

    BitArray_set(b);
    BitArray_clear_bits(b, 2, 5, 69);

    ASSERT_TRUE(BitArray_first_clear_bit(b, &result));
    ASSERT_TRUE(result == 5);
    ASSERT_TRUE(BitArray_last_clear_bit(b, &result));
    ASSERT_TRUE(result == 69);

    BitArray_free(b);
}

void test_BitArray_count_bits(void)
{
    // check table is correct (every permutaion of 8 bits [0 - 255])
    index_t size = 8;
    BitArray* b = BitArray_init(size);
    ASSERT_NOT_NULL(b);

    for (int i = 0; i <= 0xFF; ++i) {
        int set_count = 0;
        for (int j = 0; j < 8; ++j) {
            if ((0b10000000 >> j) & i) {
                BitArray_set_bit(b, j);
                ++set_count;
            } else
                 BitArray_clear_bit(b, j);
        }
        ASSERT_EQUALS(BitArray_num_set_bits(b), set_count);
        ASSERT_EQUALS(BitArray_num_clear_bits(b), 8 - set_count);
    }
    BitArray_free(b);

    size = 21;
    b = BitArray_init(size);
    ASSERT_NOT_NULL(b);

    // Check correctly counts remaining bits after full bytes
    ASSERT_EQUALS(BitArray_num_set_bits(b), 0);
    ASSERT_EQUALS(BitArray_num_clear_bits(b), size);

    BitArray_set(b);
    ASSERT_EQUALS(BitArray_num_set_bits(b), size);
    ASSERT_EQUALS(BitArray_num_clear_bits(b), 0);

    BitArray_free(b);
}

/////////////////// end of tests ///////////////////

#ifdef TESTING_ENV
#undef assert
#include <assert.h>
#endif

#include <unistd.h>  // for dup2

typedef void (*TestFunction)(void);

typedef struct {
    TestFunction function;
    const char* name;
} TestFunctionInfo;


void before_all_tests(void)
{
    // Redirect stderr to ERROR_FILE_NAME
    _error_log_fp = fopen(ERROR_FILE_NAME, "w+");
    assert(_error_log_fp != NULL);
    assert(dup2(fileno(_error_log_fp), STDERR_FILENO) != -1);
}

void after_all_tests(void)
{
    // Restore stderr and delete error log for tests
    dup2(STDERR_FILENO, fileno(stderr));
    fclose(_error_log_fp);
    remove(ERROR_FILE_NAME);

    printf("Passed %d/%d tests\n", _TEST_PASS_COUNT, _TEST_COUNT);
}

void before_each(void)
{
    _passed = true;
}

void after_each(const char* test_name)
{
    _MALLOC_FAIL = false;
    _CALLOC_FAIL = false;
    _REALLOC_FAIL = false;
    display_results(_passed, test_name, _test_line);
}

void run_tests(void)
{
    TestFunctionInfo test_funcs[] = {
        {test_BitArray_init, "test_BitArray_init"},
        {test_BitArray_set, "test_BitArray_set"},
        {test_BitArray_clear_bit, "test_BitArray_clear_bit"},
        {test_BitArray_toggle_bit, "test_BitArray_toggle_bit"},
        {test_BitArray_check_bit, "test_BitArray_check_bit"},
        {test_BitArray_init_from_hex, "test_BitArray_init_from_hex"},
        {test_BitArray_init_from_bin, "test_BitArray_init_from_bin"},
        {test_BitArray_resize, "test_BitArray_resize"},
        {test_BitArray_copy, "test_BitArray_copy"},
        {test_BitArray_modify_multiple_bits, "test_BitArray_modify_multiple_bits"},
        {test_BitArray_region_operations, "test_BitArray_region_operations"},
        {test_BitArray_operations, "test_BitArray_operations"},
        {test_BitArray_size, "test_BitArray_size"},
        {test_BitArray_count_bits, "test_BitArray_count_bits"},
        {test_BitArray_next_set_bit, "test_BitArray_next_set_bit"},
        {test_BitArray_prev_set_bit, "test_BitArray_prev_set_bit"},
        {test_BitArray_first_and_last_set, "test_BitArray_first_and_last_set"},
        {test_BitArray_next_clear_bit, "test_BitArray_next_clear_bit"},
        {test_BitArray_prev_clear_bit, "test_BitArray_prev_clear_bit"},
        {test_BitArray_first_and_last_clear, "test_BitArray_first_and_last_clear"},
        {test_BitArray_min_str_lengths, "test_BitArray_min_str_lengths"},
        {test_BitArray_to_strings, "test_BitArray_to_strings"},
        {test_BitArray_print_hex, "test_BitArray_print_hex"},
        {test_BitArray_print_bin, "test_BitArray_print_bin"},
        {test_BitArray_file_save, "test_BitArray_file_save"}
    };
    const int NUM_TESTS = sizeof(test_funcs) / sizeof(test_funcs[0]);

    // Run the test in a random order
    shuffle_array(test_funcs, sizeof(test_funcs[0]), NUM_TESTS);

    for (int i = 0; i < NUM_TESTS; ++i) {
        before_each();
        test_funcs[i].function();
        after_each(test_funcs[i].name);
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