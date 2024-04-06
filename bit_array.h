/*
* bit_array.h -- An interface for working with bit arrays
*
* Author: Max Kolenc
* Date: Jan, 2024
*
* Do this:
*   #define BIT_ARRAY_IMPLEMENTATION
* before you include this file in *one* c or c++ file to create the implementation.
*
* i.e., it should look like this:
* #include ...
* #include ...
* #include ...
* #define BIT_ARRAY_IMPLEMENTATION
* #include "bit_array.h"
*/
#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

// typedefs
typedef struct BitArray BitArray;
typedef size_t index_t;

// Initialization
// ----------------------------------------------------------------------------------------------------------

BitArray* BitArray_init(index_t size);
BitArray* BitArray_init_from_hex(const char* str);
BitArray* BitArray_init_from_bin(const char* str);

// Memory Management
// ----------------------------------------------------------------------------------------------------------

void BitArray_free(BitArray* bit_array);
BitArray* BitArray_resize(BitArray* bit_array, index_t size);
BitArray* BitArray_copy(const BitArray* bit_array);

// Bit Operations
// ----------------------------------------------------------------------------------------------------------

bool BitArray_check_bit(const BitArray* bit_array, index_t bit_index);
void BitArray_set_bit(BitArray* bit_array, index_t bit_index);
void BitArray_clear_bit(BitArray* bit_array, index_t bit_index);
void BitArray_toggle_bit(BitArray* bit_array, index_t bit_index);

// Macros
#define BitArray_set_bits(bit_array, num_bits_to_set, ...) \
    internal_BitArray_modify_bits(bit_array, BitArray_set_bit, num_bits_to_set, __VA_ARGS__)

#define BitArray_clear_bits(bit_array, num_bits_to_clear, ...) \
    internal_BitArray_modify_bits(bit_array, BitArray_clear_bit, num_bits_to_clear, __VA_ARGS__)

#define BitArray_toggle_bits(bit_array, num_bits_to_toggle, ...) \
    internal_BitArray_modify_bits(bit_array, BitArray_toggle_bit, num_bits_to_toggle, __VA_ARGS__)

// Region Operations
// ----------------------------------------------------------------------------------------------------------

void BitArray_set_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index);
void BitArray_clear_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index);
void BitArray_toggle_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index);

void BitArray_set(BitArray* bit_array);
void BitArray_clear(BitArray* bit_array);
void BitArray_toggle(BitArray* bit_array);

// Information Retrieval
// ----------------------------------------------------------------------------------------------------------

index_t BitArray_size(const BitArray* bit_array);
index_t BitArray_num_set_bits(const BitArray* bit_array);
index_t BitArray_num_clear_bits(const BitArray* bit_array);

bool BitArray_next_set_bit(const BitArray* bit_array, index_t initial_index, index_t* result);
bool BitArray_next_clear_bit(const BitArray* bit_array, index_t initial_index, index_t* result);
bool BitArray_prev_set_bit(const BitArray* bit_array, index_t initial_index, index_t* result);
bool BitArray_prev_clear_bit(const BitArray* bit_array, index_t initial_index, index_t* result);

bool BitArray_first_clear_bit(const BitArray* bit_array, index_t* result);
bool BitArray_first_set_bit(const BitArray* bit_array, index_t* result);
bool BitArray_last_clear_bit(const BitArray* bit_array, index_t* result);
bool BitArray_last_set_bit(const BitArray* bit_array, index_t* result);

// String functions
// ----------------------------------------------------------------------------------------------------------

index_t BitArray_min_hex_str_len(const BitArray* bit_array);
index_t BitArray_min_bin_str_len(const BitArray* bit_array);

char* BitArray_to_hex_str(const BitArray* bit_array, char* dst);
char* BitArray_to_bin_str(const BitArray* bit_array, char* dst);
// File Operations
// ----------------------------------------------------------------------------------------------------------

void BitArray_print_hex(const BitArray* bit_array, FILE* file_stream, index_t chars_per_line);
void BitArray_print_bin(const BitArray* bit_array, FILE* file_stream, index_t chars_per_line);

bool BitArray_save(const BitArray* bit_array, const char file_name[]);
BitArray* BitArray_load(const char file_name[]);

#endif  /* BIT_ARRAY_H */

//
//
////   end header file   /////////////////////////////////////////////////////

#ifdef BIT_ARRAY_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdint.h>

#ifndef TESTING_ENV
#include <assert.h>
#endif

// Constants
#define SET_BYTE ((uint8_t) 0XFF)
#define CLEAR_BYTE ((uint8_t) 0)
#define FILE_HEADER ("BitArray_Data_File")
#define HEADER_LEN (18)

#ifndef UINT8_WIDTH // For compatibility with versions preceding -std=c23
#define UINT8_WIDTH (8)
#endif

typedef enum SearchDirection {
    SEARCH_BACKWARD = -1,
    SEARCH_FORWARD = 1
} SearchDirection;

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




// TODO:

//////////   Misc    ////////////

// Unit test file with make file: make tests
// Update README.md

// Note: assert statements are used for checking valid indicies, and non-NULL strings.
//       If user is confident in use, can add -D NDEBUG for faster code with disabled error checking

// Add pre-conditions that lib expects non-NULL arguments, it checks for valid indicies, and resonable errors
// but blatantly passing in something like BitArray_copy(NULL) will crash / is undefined. Also things like
// File pointers are not NULL and valid files, same with bit_arrays are valid / intiilized arrays...


////////// Additional functionality ?/////////

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

/*  Math (Traditional mult and divide algorithms are slow, like super duper slow. O(n^2).
          I don't really feel like doing alot of research on faster algorithms, kinda outside scope of this project)

    - add // might do option to add string, and two bit arrays
    - sub
*/

/*  Sorting
    - ascending
    - descending
    - compare
*/


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

// No prefixes
BitArray* BitArray_init_from_hex(const char* str)
{
    assert(str != NULL);
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
    assert(str != NULL);
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

bool BitArray_check_bit(const BitArray* bit_array, index_t bit_index)
{
    assert(bit_index < bit_array->num_bits && "Bit index is out of range");
    return (bit_array->data[BYTE_INDEX(bit_index)] & GET_MASK(bit_index));
}

void BitArray_set_bit(BitArray* bit_array, index_t bit_index)
{
    assert(bit_index < bit_array->num_bits && "Bit index is out of range");
    bit_array->data[BYTE_INDEX(bit_index)] |= GET_MASK(bit_index);
}

void BitArray_clear_bit(BitArray* bit_array, index_t bit_index)
{
    assert(bit_index < bit_array->num_bits && "Bit index is out of range");
    bit_array->data[BYTE_INDEX(bit_index)] &= ~GET_MASK(bit_index);
}

void BitArray_toggle_bit(BitArray* bit_array, index_t bit_index)
{
    assert(bit_index < bit_array->num_bits && "Bit index is out of range");
    bit_array->data[BYTE_INDEX(bit_index)] ^= GET_MASK(bit_index);
}

// Note, precondition num_bit_to_modify is correct, and you pass in atleast one index
void internal_BitArray_modify_bits(BitArray* bit_array,
                                   void (*bit_operation)(BitArray*, index_t),
                                   index_t num_bits_to_modify, ...)
{
    va_list arg_list;
    va_start(arg_list, num_bits_to_modify);

    for (index_t i = 0; i < num_bits_to_modify; ++i) {
        index_t index = va_arg(arg_list, index_t);
        bit_operation(bit_array, index);
    }
    va_end(arg_list);
}

void internal_BitArray_operate_region(BitArray* bit_array,
                                             index_t start_bit_index,
                                             index_t end_bit_index,
                                             void (*bit_operation)(BitArray*, index_t))
{
    assert(start_bit_index < bit_array->num_bits &&
            end_bit_index < bit_array->num_bits && "Bit index is out of range");

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

inline void BitArray_set_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index)
{
    internal_BitArray_operate_region(bit_array, start_bit_index, end_bit_index, BitArray_set_bit);
}

inline void BitArray_clear_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index)
{
    internal_BitArray_operate_region(bit_array, start_bit_index, end_bit_index, BitArray_clear_bit);
}

inline void BitArray_toggle_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index)
{
    internal_BitArray_operate_region(bit_array, start_bit_index, end_bit_index, BitArray_toggle_bit);
}

inline void BitArray_set(BitArray* bit_array)
{
    internal_BitArray_operate_region(bit_array, 0, bit_array->num_bits - 1, BitArray_set_bit);
}

inline void BitArray_clear(BitArray* bit_array)
{
    internal_BitArray_operate_region(bit_array, 0, bit_array->num_bits - 1, BitArray_clear_bit);
}

inline void BitArray_toggle(BitArray* bit_array)
{
    internal_BitArray_operate_region(bit_array, 0, bit_array->num_bits - 1, BitArray_toggle_bit);
}

inline index_t BitArray_size(const BitArray* bit_array) {
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

inline index_t BitArray_num_clear_bits(const BitArray* bit_array)
{
    return bit_array->num_bits - BitArray_num_set_bits(bit_array);
}

// initial_index is inclusive,
bool internal_BitARray_find_bit(const BitArray* bit_array,
                                        index_t initial_index,
                                        index_t* result,
                                        BitState find_bit_state,
                                        SearchDirection search_direction)
{
    assert(initial_index < bit_array->num_bits && "Bit index is out of range"); /// Want to re-write so iterare over bytes, not bits

    index_t end_index = (search_direction == SEARCH_FORWARD) ? bit_array->num_bits : 0;

    for (index_t index = initial_index; index != end_index; index += search_direction) {
        if (BitArray_check_bit(bit_array, index) == find_bit_state) {
            *result = index;
            return true;
        }
    }

    // Handle the special case when searching backward and initial_index is 0
    if (search_direction == SEARCH_BACKWARD && initial_index == 0) {
        if (BitArray_check_bit(bit_array, 0) == find_bit_state) {
            *result = 0;
            return true;
        }
    }

    return false;
}

// If found, return true and save result in *result, else return false and dont change *result
// Index is inclusive
inline bool BitArray_next_set_bit(const BitArray* bit_array, index_t initial_index, index_t* result)
{
    return internal_BitARray_find_bit(bit_array, initial_index, result, BIT_SET, SEARCH_FORWARD);
}

inline bool BitArray_next_clear_bit(const BitArray* bit_array, index_t initial_index, index_t* result)
{
    return internal_BitARray_find_bit(bit_array, initial_index, result, BIT_CLEAR, SEARCH_FORWARD);
}

inline bool BitArray_prev_set_bit(const BitArray* bit_array, index_t initial_index, index_t* result)
{
    return internal_BitARray_find_bit(bit_array, initial_index, result, BIT_SET, SEARCH_BACKWARD);
}

inline bool BitArray_prev_clear_bit(const BitArray* bit_array, index_t initial_index, index_t* result)
{
    return internal_BitARray_find_bit(bit_array, initial_index, result, BIT_CLEAR, SEARCH_BACKWARD);
}

inline bool BitArray_first_clear_bit(const BitArray* bit_array, index_t* result)
{
    return BitArray_next_clear_bit(bit_array, 0, result);
}

inline bool BitArray_first_set_bit(const BitArray* bit_array, index_t* result)
{
    return BitArray_next_set_bit(bit_array, 0, result);
}

inline bool BitArray_last_clear_bit(const BitArray* bit_array, index_t* result)
{
    return BitArray_prev_clear_bit(bit_array, bit_array->num_bits - 1, result);
}

inline bool BitArray_last_set_bit(const BitArray* bit_array, index_t* result)
{
    return BitArray_prev_set_bit(bit_array, bit_array->num_bits - 1, result);
}

index_t BitArray_min_hex_str_len(const BitArray* bit_array)
{
    return POS_CEIL(bit_array->num_bits / 4.0) + 1;
}

index_t BitArray_min_bin_str_len(const BitArray* bit_array)
{
    return bit_array->num_bits + 1;
}

// Guarentees dst is a valid string by writing a null-terminator at the end
// dst should be atleast BitArray_min_hex_str_len();
char* BitArray_to_hex_str(const BitArray* bit_array, char* dst)
{
    assert(dst != NULL);
    char* tmp = dst;

    size_t num_nibbles = BitArray_min_hex_str_len(bit_array) - 1;
    for (index_t i = 1; i <= num_nibbles; ++i) {
        uint8_t nibble = bit_array->data[(i - 1) / 2] & (0XF << ((i & 1) * 4));
        sprintf(dst++, "%X", nibble);
    }
    *dst = '\0';

    return tmp;
}

char* BitArray_to_bin_str(const BitArray* bit_array, char* dst)
{
    assert(dst != NULL);
    char* tmp = dst;

    for (index_t i = 0; i < BitArray_min_bin_str_len(bit_array) - 1; ++i)
        *tmp++ = BitArray_check_bit(bit_array, i) + '0';
    *tmp = '\0';

    return dst;
}

void BitArray_print_hex(const BitArray* bit_array, FILE* file_stream, index_t chars_per_line)
{
    assert(chars_per_line);
    const index_t num_nibbles = POS_CEIL(bit_array->num_bits / 4.0);

    for (index_t i = 1; i <= num_nibbles; ++i) {
        uint8_t nibble = bit_array->data[(i - 1) / 2] & (0XF << ((i & 1) * 4));
        fprintf(file_stream, "%X", nibble >> (i & 1 ? 4 : 0));
            if (i != num_nibbles)
                fprintf(file_stream, "%s", (i % chars_per_line == 0) ? "\n" : ", ");
    }
    fprintf(stderr, "\n");
}

void BitArray_print_bin(const BitArray* bit_array, FILE* file_stream, index_t chars_per_line)
{
    assert(chars_per_line);

    for (index_t i = 1; i <= bit_array->num_bits; ++i) {
        fprintf(file_stream, "%c", BitArray_check_bit(bit_array, i - 1) + '0');
        if (i != bit_array->num_bits)
            fprintf(file_stream, "%s", (i % chars_per_line == 0) ? "\n" : ", ");
    }
    fprintf(stderr, "\n");
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
        fprintf(stderr, "Error initializing BitArray from '%s': %s\n", file_name, strerror(errno));
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

#endif /* BIT_ARRAY_IMPLEMENTATION */