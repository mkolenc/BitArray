#define TESTING_ENV
#include "test_macros.h"
#include "../bit_array.c"

/********************************* Start of Tests *********************************/

void test_BitArray_init(void)
{
    index_t size = 26;
    test_data.malloc_fail = true;
    CAPTURE_OUTPUT(
        ASSERT_NULL(BitArray_init(size));
    );
    ASSERT_STR_EQUAL(test_data.buffer, "Failed to allocate BitArray struct: ERROR\n");

    test_data.malloc_fail = false;
    test_data.calloc_fail = true;
    CAPTURE_OUTPUT(
        ASSERT_NULL(BitArray_init(size));
    );
    ASSERT_STR_EQUAL(test_data.buffer, "Failed to allocate BitArray data of size 26: ERROR\n");
    test_data.calloc_fail = false;

    size = 0;
    BitArray* bit_array = BitArray_init(size);
    ASSERT_NOT_NULL(bit_array);
    ASSERT_EQUALS(bit_array->num_bits, size);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), "");

    BitArray_free(bit_array);
    size = 10;
    bit_array = BitArray_init(size);

    ASSERT_NOT_NULL(bit_array);
    ASSERT_EQUALS(bit_array->num_bits, size);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), "0000000000");

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
    ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), "000000000000000000000001");

    // Setting an already set bit shouldn't modify it
    BitArray_set_bit(bit_array, 23);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), "000000000000000000000001");

    // Correctly indexes into first and last bye
    BitArray_set_bit(bit_array, 0);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), "100000000000000000000001");

    // Check all bit offsets in a byte
    char str[25] = {0};
    for (size_t i = 8; i < 16; ++i) {
        BitArray_clear(bit_array);
        BitArray_set_bit(bit_array, i);
        memset(str, '0', size);
        str[i] = '1';

        ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), str);
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
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "000000000111000000000000");
    BitArray_toggle_region(b, 8, 13);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "000000001000110000000000");
    BitArray_clear_region(b, 9, 15);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "000000001000000000000000");

    // Startin index is a MSB, ending index is LSB
    BitArray_set_region(b, 8, 23);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "000000001111111111111111");
    BitArray_toggle_region(b, 0, 15);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "111111110000000011111111");
    BitArray_clear_region(b, 15, 23);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "111111110000000000000000");

    // Starting / ending index are in middle of byte
    BitArray_toggle_region(b, 4, 12);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "111100001111100000000000");
    BitArray_set_region(b, 6, 16);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "111100111111111110000000");
    BitArray_clear_region(b, 2, 9);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "110000000011111110000000");

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
    ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), "111111111111111111111110");

    // Clearing an already cleared bit shouldn't modify it
    BitArray_clear_bit(bit_array, 23);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), "111111111111111111111110");

    // Correctly indexes into first and last bye
    BitArray_clear_bit(bit_array, 0);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), "011111111111111111111110");

    // Check all bit offsets in a byte
    char str[25] = {0};
    for (size_t i = 8; i < 16; ++i) {
        BitArray_set(bit_array);
        BitArray_clear_bit(bit_array, i);
        memset(str, '1', size);
        str[i] = '0';

        ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), str);
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
    ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), "000000000000000000000001");

    // Correctly indexes into first and last bye
    BitArray_toggle_bit(bit_array, 0);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), "100000000000000000000001");

    // Toggle every bit offset on and off
    char str[25] = {0};
    for (size_t i = 8; i < 16; ++i) {
        BitArray_clear(bit_array);

        BitArray_toggle_bit(bit_array, i);
        memset(str, '0', size);
        str[i] = '1';
        ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), str);

        BitArray_toggle_bit(bit_array, i);
        memset(str, '0', size);
        ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), str);
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

    test_data.malloc_fail = true;
    CAPTURE_OUTPUT(
        ASSERT_NULL(BitArray_init_from_hex("abd00xf"))
    );
    ASSERT_STR_EQUAL(test_data.buffer, "Failed to allocate BitArray struct: ERROR\n");

    test_data.malloc_fail = false;
    CAPTURE_OUTPUT(
        ASSERT_CRASH(BitArray_init_from_hex("adm0Xf"));
    );
    ASSERT_STR_EQUAL(test_data.buffer, "Invalid hex string\n");

    // good wheather tests
    BitArray* bit_array = BitArray_init_from_hex("");
    ASSERT_NOT_NULL(bit_array);
    ASSERT_TRUE(bit_array->num_bits == 0);

    ASSERT_STR_EQUAL(BitArray_to_hex_str(bit_array, test_data.buffer), "");

    bit_array = BitArray_init_from_hex("0123456789aBcDeF");
    ASSERT_NOT_NULL(bit_array);
    ASSERT_TRUE(bit_array->num_bits == 64);
    ASSERT_STR_EQUAL(BitArray_to_hex_str(bit_array, test_data.buffer), "0123456789ABCDEF");

    BitArray_free(bit_array);
}

void test_BitArray_init_from_bin(void)
{
    // Bad wheather tests
    ASSERT_CRASH(BitArray_init_from_bin(NULL));

    test_data.malloc_fail = true;
    CAPTURE_OUTPUT(
        ASSERT_NULL(BitArray_init_from_bin("010101"))
    );
    ASSERT_STR_EQUAL(test_data.buffer, "Failed to allocate BitArray struct: ERROR\n");

    test_data.malloc_fail = false;
    // Invalid hex chars
    CAPTURE_OUTPUT(
        ASSERT_CRASH(BitArray_init_from_bin("0101ff"));
    );
    ASSERT_STR_EQUAL(test_data.buffer, "Invalid binary string.\n");

    // Invalid digits
    CAPTURE_OUTPUT(
        ASSERT_CRASH(BitArray_init_from_bin("01012005"));
    );
    ASSERT_STR_EQUAL(test_data.buffer, "Invalid binary string.\n");


    // good wheather tests
    BitArray* bit_array = BitArray_init_from_bin("");
    ASSERT_NOT_NULL(bit_array);
    ASSERT_TRUE(bit_array->num_bits == 0);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), "");

    bit_array = BitArray_init_from_bin("00000010101010");
    ASSERT_NOT_NULL(bit_array);
    ASSERT_TRUE(bit_array->num_bits == 14);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(bit_array, test_data.buffer), "00000010101010");

    BitArray_free(bit_array);
}

void test_BitArray_resize(void)
{
    index_t size = 64;
    BitArray* b = BitArray_init(size);
    ASSERT_NOT_NULL(b);

    ASSERT_NULL(BitArray_resize(b, 0));

    test_data.realloc_fail = true;
    CAPTURE_OUTPUT(
        ASSERT_NULL(BitArray_resize(b, 1902));
    );
    ASSERT_STR_EQUAL(test_data.buffer, "Unable to resize BitArray to size 1902 bits: ERROR\n");
    test_data.realloc_fail = false;

    BitArray_set(b);

    // Resize to same size (should do nothing)
    ASSERT_EQUALS(BitArray_resize(b, size), b);
    ASSERT_EQUALS(b->num_bits, size);
    ASSERT_STR_EQUAL(BitArray_to_hex_str(b, test_data.buffer), "FFFFFFFFFFFFFFFF");

    // Decrease size
    size = 24;
    ASSERT_EQUALS(BitArray_resize(b, size), b);
    ASSERT_EQUALS(b->num_bits, size);
    ASSERT_STR_EQUAL(BitArray_to_hex_str(b, test_data.buffer), "FFFFFF");

    // Increase size, zeros should be added
    size = 64;
    ASSERT_EQUALS(BitArray_resize(b, size), b);
    ASSERT_EQUALS(b->num_bits, size);
    ASSERT_STR_EQUAL(BitArray_to_hex_str(b, test_data.buffer), "FFFFFF0000000000");

    BitArray_free(b);
}

void test_BitArray_copy(void)
{
    BitArray* b = BitArray_init_from_hex("AB2255657B7B756DAA083");
    ASSERT_NOT_NULL(b);

    test_data.malloc_fail = true;
    CAPTURE_OUTPUT(
        ASSERT_NULL(BitArray_copy(b));
    );
    test_data.malloc_fail = false;

    BitArray* copy = BitArray_copy(b);
    ASSERT_STR_EQUAL(BitArray_to_hex_str(copy, test_data.buffer), "AB2255657B7B756DAA083");


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
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "1001010001");

    BitArray_clear_bits(b, 2, 0, 9);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "0001010000");

    BitArray_toggle_bits(b, 5, 2, 3, 4, 5, 6);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "0010101000");

    BitArray_free(b);
}

void test_BitArray_print_hex(void)
{
    BitArray* b = BitArray_init_from_hex("0123456789ABCDEF");
    ASSERT_NOT_NULL(b);

    ASSERT_CRASH(BitArray_print_hex(b, stderr, 0));

    CAPTURE_OUTPUT(
        BitArray_print_hex(b, stderr, 16);
    );
    ASSERT_STR_EQUAL(test_data.buffer, "0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F\n");

    CAPTURE_OUTPUT(
        BitArray_print_hex(b, stderr, 15);
    );
    ASSERT_STR_EQUAL(test_data.buffer, "0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E\nF\n");

    CAPTURE_OUTPUT(
        BitArray_print_hex(b, stderr, 3);
    );
    ASSERT_STR_EQUAL(test_data.buffer, "0, 1, 2\n3, 4, 5\n6, 7, 8\n9, A, B\nC, D, E\nF\n");

    BitArray_free(b);

    // Check the case where last nibble is forward-padded with zeros
    b = BitArray_init_from_bin("1010111"); // [1010, 111]
    ASSERT_NOT_NULL(b);

    CAPTURE_OUTPUT(
        BitArray_print_hex(b, stderr, 3);
    );
    ASSERT_STR_EQUAL(test_data.buffer, "A, 7\n");

    BitArray_free(b);
}

void test_BitArray_file_save(void)
{
    // Load should fail on non-BitArray data
    const char* tmp = "some_text.txt";
    FILE* fp = fopen(tmp, "w");
    ASSERT_NOT_NULL(fp);
    fputs("Shall I compare thee to a summers day?\n", fp);
    fclose(fp);

    BitArray* b;
    CAPTURE_OUTPUT(
        b = BitArray_load(tmp);
    );
    ASSERT_STR_EQUAL(test_data.buffer, "some_text.txt does not contain BitArray data.\n");

    const char* hex_str = "ADF3527FA0009382777D7A73625";
    b = BitArray_init_from_hex(hex_str);
    ASSERT_NOT_NULL(b);

    ASSERT_TRUE(BitArray_save(b, tmp));
    BitArray_free(b);

    BitArray* loaded = BitArray_load(tmp);
    ASSERT_NOT_NULL(loaded);

    ASSERT_STR_EQUAL(BitArray_to_hex_str(loaded, test_data.buffer), hex_str);

    BitArray_free(loaded);
    remove(tmp);
}

void test_BitArray_print_bin(void)
{
    BitArray* b = BitArray_init_from_bin("0100001010101011");
    ASSERT_NOT_NULL(b);

    ASSERT_CRASH(BitArray_print_bin(b, stderr, 0));

    CAPTURE_OUTPUT(
        BitArray_print_bin(b, stderr, 16);
    );
    ASSERT_STR_EQUAL(test_data.buffer, "0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1\n");

    CAPTURE_OUTPUT(
        BitArray_print_bin(b, stderr, 15);
    );
    ASSERT_STR_EQUAL(test_data.buffer, "0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1\n1\n");

    CAPTURE_OUTPUT(
        BitArray_print_bin(b, stderr, 2);
    );
    ASSERT_STR_EQUAL(test_data.buffer, "0, 1\n0, 0\n0, 0\n1, 0\n1, 0\n1, 0\n1, 0\n1, 1\n");

    BitArray_free(b);
}

void test_BitArray_operations(void)
{
    index_t size = 20;
    BitArray* b = BitArray_init(size);
    ASSERT_NOT_NULL(b);

    BitArray_set(b);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "11111111111111111111");

    BitArray_clear(b);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "00000000000000000000");

    BitArray_set_bits(b, 3, 2, 12, 16);
    BitArray_toggle(b);
    ASSERT_STR_EQUAL(BitArray_to_bin_str(b, test_data.buffer), "11011111111101110111");

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

    ASSERT_NOT_NULL(BitArray_to_bin_str(b, test_data.buffer));
    ASSERT_STR_EQUAL(test_data.buffer, "");
    ASSERT_NOT_NULL(BitArray_to_hex_str(b, test_data.buffer));
    ASSERT_STR_EQUAL(test_data.buffer, "");

    BitArray_free(b);

    const char* bin_str = "0000000100100011010001010110011110001001101010111100110111101111";
    const char* hex_str = "0123456789ABCDEF";

    b = BitArray_init_from_hex(hex_str);
    ASSERT_NOT_NULL(b);

    ASSERT_NOT_NULL(BitArray_to_bin_str(b, test_data.buffer));
    ASSERT_STR_EQUAL(test_data.buffer, bin_str);
    ASSERT_NOT_NULL(BitArray_to_hex_str(b, test_data.buffer));
    ASSERT_STR_EQUAL(test_data.buffer, hex_str);

    BitArray_free(b);

    // Check the case where last nibble is forward-padded with zeros
    b = BitArray_init_from_bin("1010111"); // [1010, 111]
    ASSERT_NOT_NULL(b);
    ASSERT_STR_EQUAL(BitArray_to_hex_str(b, test_data.buffer), "A7");

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
            if ((0x80 >> j) & i) {
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

/********************************* End of Tests *********************************/
#include "test_utils.h"

typedef struct {
    void (*function)(void);
    const char* name;
} TestFunctionInfo;

typedef struct {
    TestFunctionInfo* tests;
    size_t num_tests;
    size_t num_passed;
    clock_t start_time;
    clock_t end_time;
} TestSuite;

void before_all(TestSuite* suite)
{
    shuffle_array(suite->tests, sizeof(suite->tests[0]), suite->num_tests);
    suite->start_time = clock();
}

void after_all(TestSuite* suite)
{
    suite->end_time = clock();
    remove(TESTING_OUTPUT_FILE_NAME);

    display_test_summary(suite->num_tests, suite->num_passed, suite->start_time, suite->end_time);
}

void before_each(void)
{
    test_data.passed = true;
    test_data.malloc_fail = false;
    test_data.calloc_fail = false;
    test_data.realloc_fail = false;
}

void after_each(TestSuite* suite, size_t i)
{
    if (test_data.passed)
        suite->num_passed++;

    display_test_result(suite->tests[i].name, test_data.passed, test_data.line_number);
}

void run_tests(TestSuite* suite)
{
    for (size_t i = 0; i < suite->num_tests; ++i) {
        before_each();
        suite->tests[i].function();
        after_each(suite, i);
    }
}

int main(void)
{
    TestFunctionInfo tests[] = {
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

    TestSuite suite = {tests, sizeof(tests) / sizeof(tests[0]), 0, 0, 0};

    before_all(&suite);
    run_tests(&suite);
    after_all(&suite);

    return 0;
}