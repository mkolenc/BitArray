/*
* bit_array.c -- An implementaion of a bit array data structure
*
* Author: Max Kolenc
* Date: Jan, 2024
*/

// TODO:

/*  bitwise / logical operations:
    - and
    - or
    - xor
    - not
    - ror
    - rol
    - lsl
    - lsr
*/

/*  Arithmetic
    - add
    - sub
    - mult ?
    - divide ?
*/

/*  Sorting
    - ascending
    - descending
    - compare
*/

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdint.h>
#include "bit_array.h"

#ifndef TESTING_ENV
#include <assert.h>
#endif

// Optionally remove error checking for a potential speed up
#ifdef BIT_ARRAY_FAST
    #define BITARRAY_ASSERT_INDEX_IN_RANGE(bit_array, bit_index) ((void) 0)
    #define BITARRAY_ASSERT_TRUE(condition) ((void) 0)
#else
    #define BITARRAY_ASSERT_INDEX_IN_RANGE(bit_array, bit_index) assert((bit_index) < (bit_array)->num_bits && "Bit index is out of range")
    #define BITARRAY_ASSERT_TRUE(condition) assert(condition)
#endif /* BIT_ARRAY_FAST */

// Constants
#define SET_BYTE ((uint8_t) 0XFF)
#define CLEAR_BYTE ((uint8_t) 0)
#define FILE_HEADER ("BitArray_Data_File")
#define HEADER_LEN (18)

#ifndef UINT8_WIDTH // For compatibility with versions preceding -std=c23
#define UINT8_WIDTH (8)
#endif

typedef enum BitState {
    BIT_CLEAR = 0,
    BIT_SET = 1
} BitState;

// Macro functions
#define BYTES_FROM_BITS(bits) POS_CEIL(((long double) bits) / UINT8_WIDTH)
#define BYTE_INDEX(index) ((index) / UINT8_WIDTH)
#define BIT_OFFSET(index) ((index) % UINT8_WIDTH)
#define GET_MASK(index) (1 << (UINT8_WIDTH - BIT_OFFSET(index) - 1))

#define POS_CEIL(x) (((x) > (index_t)(x)) ? ((index_t)(x) + 1) : ((index_t)(x)))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

// Structure
struct BitArray {
    index_t num_bits;
    uint8_t* data;
};

BitArray* BitArray_init(index_t size)
{
    BitArray* bit_array = malloc(sizeof(*bit_array));
    if (bit_array == NULL) {
        fprintf(stderr, "Failed to allocate BitArray struct: %s\n", strerror(errno));
        return NULL;
    }
    bit_array->data = calloc(BYTES_FROM_BITS(size), 1);
    if (bit_array->data == NULL) {
        fprintf(stderr, "Failed to allocate BitArray data of size %zu: %s\n", size, strerror(errno));
        free(bit_array);
        return NULL;
    }
    bit_array->num_bits = size;

    return bit_array;
}

BitArray* BitArray_init_from_hex(const char* str)
{
    BITARRAY_ASSERT_TRUE(str != NULL);
    index_t digits = strlen(str);

    BitArray* bit_array = BitArray_init(digits * 4);
    if (bit_array == NULL)
        return NULL;

    for (index_t i = 1; i <= digits; ++i, ++str) {
        uint8_t val = *str;

        if (isdigit(val))
            val -= '0';
        else if (('A' <= (val = toupper(val))) && val <= 'F')
            val = 10 + (val - 'A');
        else {
            fprintf(stderr, "Invalid hex string\n");
            exit(EXIT_FAILURE);
        }

        // Sets upper / lower nibbles of each byte
        bit_array->data[(i - 1) / 2] |= val << ((i & 1) * 4);
    }

    return bit_array;
}

BitArray* BitArray_init_from_bin(const char* str)
{
    BITARRAY_ASSERT_TRUE(str != NULL);
    index_t digits = strlen(str);

    BitArray* bit_array = BitArray_init(digits);
    if (bit_array == NULL)
        return NULL;

    for (index_t i = 0; i < digits; ++i, ++str) {
        uint8_t val = *str;

        if (!isdigit(val)) {
            fprintf(stderr, "Invalid binary string.\n");
            exit(EXIT_FAILURE);
        }
        if (val == '0')
            BitArray_clear_bit(bit_array, i);
        else
            BitArray_set_bit(bit_array, i);
    }

    return bit_array;
}

void BitArray_free(BitArray* bit_array)
{
    free(bit_array->data);
    free(bit_array);
}

BitArray* BitArray_resize(BitArray* bit_array, index_t size)
{
    if (size == 0)
        return NULL;

    uint8_t* tmp = realloc(bit_array->data, BYTES_FROM_BITS(size));
    if (tmp == NULL) {
        fprintf(stderr, "Unable to resize BitArray to size %zu bits: %s\n", size, strerror(errno));
        return NULL;
    }

    index_t old_size = bit_array->num_bits;
    bit_array->data = tmp;
    bit_array->num_bits = size;

    // If extending, clear additional memory
    if (size > old_size)
        BitArray_clear_region(bit_array, old_size, size - 1);

    return bit_array;
}

BitArray* BitArray_copy(const BitArray* bit_array)
{
    BitArray* new_bit_array = BitArray_init(bit_array->num_bits);
    if (new_bit_array == NULL)
        return NULL;

    memcpy(new_bit_array->data, bit_array->data, BYTES_FROM_BITS(bit_array->num_bits));

    return new_bit_array;
}

// Return true if set false otherwise.
extern inline bool BitArray_check_bit(const BitArray* bit_array, index_t bit_index)
{
    BITARRAY_ASSERT_INDEX_IN_RANGE(bit_array, bit_index);
    return (bit_array->data[BYTE_INDEX(bit_index)] & GET_MASK(bit_index));
}

extern inline void BitArray_set_bit(BitArray* bit_array, index_t bit_index)
{
    BITARRAY_ASSERT_INDEX_IN_RANGE(bit_array, bit_index);
    bit_array->data[BYTE_INDEX(bit_index)] |= GET_MASK(bit_index);
}

extern inline void BitArray_clear_bit(BitArray* bit_array, index_t bit_index)
{
    BITARRAY_ASSERT_INDEX_IN_RANGE(bit_array, bit_index);
    bit_array->data[BYTE_INDEX(bit_index)] &= ~GET_MASK(bit_index);
}

extern inline void BitArray_toggle_bit(BitArray* bit_array, index_t bit_index)
{
    BITARRAY_ASSERT_INDEX_IN_RANGE(bit_array, bit_index);
    bit_array->data[BYTE_INDEX(bit_index)] ^= GET_MASK(bit_index);
}

void BitArray_set_bits(BitArray* bit_array, index_t num_bits_to_set, ...)
{
    va_list arg_list;
    va_start(arg_list, num_bits_to_set);

    for (index_t i = 0; i < num_bits_to_set; ++i) {
        index_t index = va_arg(arg_list, index_t);
        BitArray_set_bit(bit_array, index);
    }
    va_end(arg_list);
}

void BitArray_clear_bits(BitArray* bit_array, index_t num_bits_to_clear, ...)
{
    va_list arg_list;
    va_start(arg_list, num_bits_to_clear);

    for (index_t i = 0; i < num_bits_to_clear; ++i) {
        index_t index = va_arg(arg_list, index_t);
        BitArray_clear_bit(bit_array, index);
    }
    va_end(arg_list);
}

void BitArray_toggle_bits(BitArray* bit_array, index_t num_bits_to_toggle, ...)
{
    va_list arg_list;
    va_start(arg_list, num_bits_to_toggle);

    for (index_t i = 0; i < num_bits_to_toggle; ++i) {
        index_t index = va_arg(arg_list, index_t);
        BitArray_toggle_bit(bit_array, index);
    }
    va_end(arg_list);
}

// Sets, clears or toggles a region from start_bit_index to end_bit_index inclusive.
static void internal_BitArray_operate_region(BitArray* bit_array,
                                             index_t start_bit_index,
                                             index_t end_bit_index,
                                             void (*bit_operation)(BitArray*, index_t))
{
    BITARRAY_ASSERT_INDEX_IN_RANGE(bit_array, start_bit_index);
    BITARRAY_ASSERT_INDEX_IN_RANGE(bit_array, end_bit_index);

    start_bit_index = MIN(start_bit_index, end_bit_index);
    end_bit_index = MAX(start_bit_index, end_bit_index);

    index_t start_byte = BYTE_INDEX(start_bit_index);
    index_t end_byte = BYTE_INDEX(end_bit_index);

    if ((start_byte == end_byte)) {
        for (index_t i = start_bit_index; i <= end_bit_index; ++i)
            bit_operation(bit_array, i);
        return;
    }

    // Operate on bits in the first byte
    uint8_t start_bit_offset = BIT_OFFSET(start_bit_index);
    if (start_bit_offset != 0) {
        for (uint8_t i = start_bit_offset; i < UINT8_WIDTH; ++i)
            bit_operation(bit_array, start_bit_index++);

        ++start_byte;
    }

    // Operate on bits in the last byte
    uint8_t end_bit_offset = BIT_OFFSET(end_bit_index);
    if (end_bit_offset != UINT8_WIDTH - 1) {
        for (int8_t i = end_bit_offset; i >= 0; --i)
            bit_operation(bit_array, end_bit_index--);

        --end_byte;
    }

    // Operate on the remaining bytes
    uint8_t* ptr = bit_array->data + start_byte;
    index_t remaining_bytes = end_byte - start_byte + 1;

    if (bit_operation == BitArray_clear_bit)
        memset(ptr, CLEAR_BYTE, remaining_bytes);
    else if (bit_operation == BitArray_set_bit)
        memset(ptr, SET_BYTE, remaining_bytes);
    else
        for (index_t i = 0; i < remaining_bytes; ++i)
            ptr[i] ^= SET_BYTE;
}

extern inline void BitArray_set_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index)
{
    internal_BitArray_operate_region(bit_array, start_bit_index, end_bit_index, BitArray_set_bit);
}

extern inline void BitArray_clear_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index)
{
    internal_BitArray_operate_region(bit_array, start_bit_index, end_bit_index, BitArray_clear_bit);
}

extern inline void BitArray_toggle_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index)
{
    internal_BitArray_operate_region(bit_array, start_bit_index, end_bit_index, BitArray_toggle_bit);
}

extern inline void BitArray_set(BitArray* bit_array)
{
    internal_BitArray_operate_region(bit_array, 0, bit_array->num_bits - 1, BitArray_set_bit);
}

extern inline void BitArray_clear(BitArray* bit_array)
{
    internal_BitArray_operate_region(bit_array, 0, bit_array->num_bits - 1, BitArray_clear_bit);
}

extern inline void BitArray_toggle(BitArray* bit_array)
{
    internal_BitArray_operate_region(bit_array, 0, bit_array->num_bits - 1, BitArray_toggle_bit);
}

extern inline index_t BitArray_size(const BitArray* bit_array) {
    return bit_array->num_bits;
}

index_t BitArray_num_set_bits(const BitArray* bit_array)
{
    index_t num_full_bytes = bit_array->num_bits / UINT8_WIDTH;
    index_t BitArray_num_set_bits = 0;

    for (index_t i = 0; i < num_full_bytes; ++i) {
        // Used to map a uint8_t to the number of 'ON' bits
        static const uint8_t bits_set_count_table[SET_BYTE + 1] = {
            0x0, 0x1, 0x1, 0x2, 0x1, 0x2, 0x2, 0x3, 0x1, 0x2, 0x2, 0x3, 0x2, 0x3, 0x3,
            0x4, 0x1, 0x2, 0x2, 0x3, 0x2, 0x3, 0x3, 0x4, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4,
            0x4, 0x5, 0x1, 0x2, 0x2, 0x3, 0x2, 0x3, 0x3, 0x4, 0x2, 0x3, 0x3, 0x4, 0x3,
            0x4, 0x4, 0x5, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4, 0x4, 0x5, 0x3, 0x4, 0x4, 0x5,
            0x4, 0x5, 0x5, 0x6, 0x1, 0x2, 0x2, 0x3, 0x2, 0x3, 0x3, 0x4, 0x2, 0x3, 0x3,
            0x4, 0x3, 0x4, 0x4, 0x5, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4, 0x4, 0x5, 0x3, 0x4,
            0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4, 0x4, 0x5, 0x3,
            0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x3, 0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6,
            0x4, 0x5, 0x5, 0x6, 0x5, 0x6, 0x6, 0x7, 0x1, 0x2, 0x2, 0x3, 0x2, 0x3, 0x3,
            0x4, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4, 0x4, 0x5, 0x2, 0x3, 0x3, 0x4, 0x3, 0x4,
            0x4, 0x5, 0x3, 0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x2, 0x3, 0x3, 0x4, 0x3,
            0x4, 0x4, 0x5, 0x3, 0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x3, 0x4, 0x4, 0x5,
            0x4, 0x5, 0x5, 0x6, 0x4, 0x5, 0x5, 0x6, 0x5, 0x6, 0x6, 0x7, 0x2, 0x3, 0x3,
            0x4, 0x3, 0x4, 0x4, 0x5, 0x3, 0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x3, 0x4,
            0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x4, 0x5, 0x5, 0x6, 0x5, 0x6, 0x6, 0x7, 0x3,
            0x4, 0x4, 0x5, 0x4, 0x5, 0x5, 0x6, 0x4, 0x5, 0x5, 0x6, 0x5, 0x6, 0x6, 0x7,
            0x4, 0x5, 0x5, 0x6, 0x5, 0x6, 0x6, 0x7, 0x5, 0x6, 0x6, 0x7, 0x6, 0x7, 0x7,
            0x8
        };
        BitArray_num_set_bits += bits_set_count_table[bit_array->data[i]];
    }

    // Check the remaining bits in the last byte
    index_t curr_bit_index = num_full_bytes * UINT8_WIDTH;
    index_t remaining_bits = bit_array->num_bits - curr_bit_index;

    for (index_t i = 0; i < remaining_bits; ++i)
        if (BitArray_check_bit(bit_array, curr_bit_index++))
            ++BitArray_num_set_bits;

    return BitArray_num_set_bits;
}

extern inline index_t BitArray_num_clear_bits(const BitArray* bit_array)
{
    return bit_array->num_bits - BitArray_num_set_bits(bit_array);
}

// If found, return true and save result in *result, else return false and dont change *result
// Index is inclusive
static bool internal_BitArray_find_next(const BitArray* bit_array, index_t initial_index, index_t* result, BitState bit_state)
{
    BITARRAY_ASSERT_INDEX_IN_RANGE(bit_array, initial_index);

    for (index_t byte = BYTE_INDEX(initial_index); byte < BYTES_FROM_BITS(bit_array->num_bits); ++byte) {
        // Find the first byte containing a bit_state bit
        if ((bit_array->data[byte] > 0 && bit_state) ||
            (bit_array->data[byte] < 0XFF && !bit_state)) {
            index_t bit_index = byte * UINT8_WIDTH;
            uint8_t i = (byte == BYTE_INDEX(initial_index)) ? BIT_OFFSET(initial_index) : 0;

            // Iterate through the bits of the byte
            for (; (i < UINT8_WIDTH) && (bit_index + i < bit_array->num_bits); ++i) {
                if (BitArray_check_bit(bit_array, bit_index + i) == bit_state) {
                    *result = bit_index + i;
                    return true;
                }
            }
        }
    }

    return false;
}

extern inline bool BitArray_next_set_bit(const BitArray* bit_array, index_t initial_index, index_t* result)
{
    return internal_BitArray_find_next(bit_array, initial_index, result, BIT_SET);
}

extern inline bool BitArray_next_clear_bit(const BitArray* bit_array, index_t initial_index, index_t* result)
{
    return internal_BitArray_find_next(bit_array, initial_index, result, BIT_CLEAR);
}

// If found, return true and save result in *result, else return false and dont change *result
// Index is inclusive
static bool internal_BitArray_find_prev(const BitArray* bit_array, index_t initial_index, index_t* result, BitState bit_state)
{
    BITARRAY_ASSERT_INDEX_IN_RANGE(bit_array, initial_index);
    index_t byte = BYTE_INDEX(initial_index);

    for (; byte  > 0; --byte) {
        // Find the first byte containing a bit_state bit
        if ((bit_array->data[byte] > 0 && bit_state) ||
            (bit_array->data[byte] < 0XFF && !bit_state)) {
            index_t bit_index = byte * UINT8_WIDTH;
            char i = (byte == BYTE_INDEX(initial_index)) ? BIT_OFFSET(initial_index) : UINT8_WIDTH - 1;

            // Iterate through the bits of the byte
            for (; i >= 0; --i) {
                if (BitArray_check_bit(bit_array, bit_index + i) == bit_state) {
                    *result = bit_index + i;
                    return true;
                }
            }
        }
    }

    // Check the 0th index separately
    char i = (byte == 0) ? BIT_OFFSET(initial_index) : UINT8_WIDTH - 1;
    for (; i >= 0; --i) {
        if (BitArray_check_bit(bit_array, i) == bit_state) {
            *result = i;
            return true;
        }
    }

    return false;
}

extern inline bool BitArray_prev_set_bit(const BitArray* bit_array, index_t initial_index, index_t* result)
{
    return internal_BitArray_find_prev(bit_array, initial_index, result, BIT_SET);
}

extern inline bool BitArray_prev_clear_bit(const BitArray* bit_array, index_t initial_index, index_t* result)
{
    return internal_BitArray_find_prev(bit_array, initial_index, result, BIT_CLEAR);
}

extern inline bool BitArray_first_clear_bit(const BitArray* bit_array, index_t* result)
{
    return BitArray_next_clear_bit(bit_array, 0, result);
}

extern inline bool BitArray_first_set_bit(const BitArray* bit_array, index_t* result)
{
    return BitArray_next_set_bit(bit_array, 0, result);
}

extern inline bool BitArray_last_clear_bit(const BitArray* bit_array, index_t* result)
{
    return BitArray_prev_clear_bit(bit_array, bit_array->num_bits - 1, result);
}

extern inline bool BitArray_last_set_bit(const BitArray* bit_array, index_t* result)
{
    return BitArray_prev_set_bit(bit_array, bit_array->num_bits - 1, result);
}

extern inline index_t BitArray_min_hex_str_len(const BitArray* bit_array)
{
    return POS_CEIL(bit_array->num_bits / 4.0) + 1;
}

extern inline index_t BitArray_min_bin_str_len(const BitArray* bit_array)
{
    return bit_array->num_bits + 1;
}

char* BitArray_to_hex_str(const BitArray* bit_array, char* dst)
{
    BITARRAY_ASSERT_TRUE(dst != NULL);
    if (bit_array->num_bits == 0) {
        *dst = '\0';
        return dst;
    }

    char* tmp = dst;

    index_t num_nibbles = BitArray_min_hex_str_len(bit_array) - 1;
    index_t i = 1;
    for (; i < num_nibbles; ++i) {
        uint8_t nibble = bit_array->data[(i - 1) / 2] & (0X0F << ((i & 1) * 4));
        sprintf(dst++, "%X", nibble);
    }

    // In the case the BitArray is not a multiple of 4, need to left-pad zeros
    // [111] should be interpreded as [0111] not [1110].
    uint8_t nibble = bit_array->data[(i - 1) / 2] & (0X0F << ((i & 1) * 4));
    uint8_t bits_in_last_nibble = bit_array->num_bits % 4;
    if (bits_in_last_nibble)
        nibble >>= 4 - bits_in_last_nibble;

    sprintf(dst++, "%X\n", nibble >> (i & 1 ? 4 : 0));
    *dst = '\0';

    return tmp;
}

char* BitArray_to_bin_str(const BitArray* bit_array, char* dst)
{
    BITARRAY_ASSERT_TRUE(dst != NULL);
    char* tmp = dst;

    for (index_t i = 0; i < BitArray_min_bin_str_len(bit_array) - 1; ++i)
        *tmp++ = BitArray_check_bit(bit_array, i) + '0';
    *tmp = '\0';

    return dst;
}

void BitArray_print_hex(const BitArray* bit_array, FILE* file_stream, index_t chars_per_line)
{
    BITARRAY_ASSERT_TRUE(chars_per_line);
    if (bit_array->num_bits == 0)
        return;

    const index_t num_nibbles = POS_CEIL(bit_array->num_bits / 4.0);

    index_t i = 1;
    for (; i < num_nibbles; ++i) {
        // Gets the lower / upper nibble of each byte
        uint8_t nibble = bit_array->data[(i - 1) / 2] & (0X0F << ((i & 1) * 4));
        // Prints the nibble in hex, followed by a '\n' or ', '
        fprintf(file_stream, "%X%s", nibble >> (i & 1 ? 4 : 0), (i % chars_per_line == 0) ? "\n" : ", ");
    }

    // In the case the BitArray is not a multiple of 4, need to left-pad zeros
    // [111] should be interpreded as [0111] not [1110].
    uint8_t nibble = bit_array->data[(i - 1) / 2] & (0X0F << ((i & 1) * 4));
    uint8_t bits_in_last_nibble = bit_array->num_bits % 4;
    if (bits_in_last_nibble)
        nibble >>= 4 - bits_in_last_nibble;

    fprintf(file_stream, "%X\n", nibble >> (i & 1 ? 4 : 0));
}

void BitArray_print_bin(const BitArray* bit_array, FILE* file_stream, index_t chars_per_line)
{
    BITARRAY_ASSERT_TRUE(chars_per_line);

    for (index_t i = 1; i <= bit_array->num_bits; ++i) {
        fprintf(file_stream, "%c", BitArray_check_bit(bit_array, i - 1) + '0');
        if (i != bit_array->num_bits)
            fprintf(file_stream, "%s", (i % chars_per_line == 0) ? "\n" : ", ");
    }
    fprintf(file_stream, "\n");
}

bool BitArray_save(const BitArray* bit_array, const char file_name[])
{
    FILE* fp = fopen(file_name, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Error saving BitArray '%s': %s\n", file_name, strerror(errno));
        return false;
    }

    bool result = true;
    const index_t num_bytes = BYTES_FROM_BITS(bit_array->num_bits);

    if ((fwrite(FILE_HEADER, sizeof(char), HEADER_LEN, fp) != HEADER_LEN) ||
        (fwrite(&bit_array->num_bits, sizeof(index_t), 1, fp) != 1) ||
        (fwrite(bit_array->data, sizeof(uint8_t), num_bytes, fp) != num_bytes)) {
        fprintf(stderr, "Error saving BitArray '%s': %s\n", file_name, strerror(errno));
        result = false;
    }

    if (fclose(fp) == EOF) {
        fprintf(stderr, "Error closing file after saving BitArray '%s': %s\n", file_name, strerror(errno));
        result = false;
    }

    return result;
}

BitArray* BitArray_load(const char file_name[])
{
    char file_header[HEADER_LEN + 1] = {0};

    FILE* fp = fopen(file_name, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open '%s': %s\n", file_name, strerror(errno));
        return NULL;
    }

    if ((fread(file_header, sizeof(char), HEADER_LEN, fp) != HEADER_LEN) ||
        (strcmp(file_header, FILE_HEADER) != 0)) {
        fprintf(stderr, "%s does not contain BitArray data.\n", file_name);
        fclose(fp);
        return NULL;
    }

    index_t num_bits;
    if (fread(&num_bits, sizeof(index_t), 1, fp) != 1) {
        fprintf(stderr, "Error reading BitArray size from '%s': %s\n", file_name, strerror(errno));
        fclose(fp);
        return NULL;
    }

    BitArray* bit_array = BitArray_init(num_bits);
    if (bit_array == NULL) {
        fclose(fp);
        return NULL;
    }

    index_t data_bytes = BYTES_FROM_BITS(num_bits);
    if (fread(bit_array->data, sizeof(uint8_t), data_bytes, fp) != data_bytes) {
        fprintf(stderr, "Error reading BitArray data from '%s': %s\n", file_name, strerror(errno));
        fclose(fp);
        free(bit_array);
        return NULL;
    }

    if (fclose(fp) == EOF) {
        fprintf(stderr, "Error closing '%s': %s\n", file_name, strerror(errno));
        free(bit_array);
        return NULL;
    }

    return bit_array;
}