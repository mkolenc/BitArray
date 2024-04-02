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

// Constants
enum BitState {
    BIT_CLEAR,
    BIT_SET
};

// typedefs
typedef struct BitArray BitArray;
typedef size_t index_t;
typedef enum BitState BitState;

// Initialization
// ----------------------------------------------------------------------------------------------------------

BitArray* BitArray_init(index_t size);
BitArray* BitArray_init_from_hex(const char* str);
BitArray* BitArray_init_from_dec(const char* str);
BitArray* BitArray_init_from_bin(const char* str);

// Memory Management
// ----------------------------------------------------------------------------------------------------------

void BitArray_free(BitArray* bit_array);
BitArray* BitArray_resize(BitArray* bit_array, index_t size);
BitArray* BitArray_copy(const BitArray* bit_array);

// Bit Operations
// ----------------------------------------------------------------------------------------------------------

BitState BitArray_check_bit(const BitArray* bit_array, index_t bit_index);
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

index_t BitArray_next_set_bit(const BitArray* bit_array, index_t initial_index);
index_t BitArray_next_clear_bit(const BitArray* bit_array, index_t initial_index);
index_t BitArray_prev_set_bit(const BitArray* bit_array, index_t initial_index);
index_t BitArray_prev_clear_bit(const BitArray* bit_array, index_t initial_index);

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
#include <assert.h>
#include <ctype.h>
#include <stdint.h>

// Constants
#define SET_BYTE ((uint8_t) 0XFF)
#define CLEAR_BYTE ((uint8_t) 0)
#define FILE_HEADER ("BitArray_Data_File")
#define HEADER_LEN (18)
#define UINT8_WIDTH (8)

typedef enum SearchDirection {
    SEARCH_BACKWARD = -1,
    SEARCH_FORWARD = 1
} SearchDirection;

typedef enum NumType {
    HEXADECIMAL,
    DECIMAL,
    BINARY
} NumType;

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


////////// Additional functionality ?/////////

/*  String functions: (also need to implement for testing)
    - to_hex_str
    - to_bin_str
    - min_str_len (minimum lenght array needed to save bit array as hex/bin string)
*/

/*  quick add to infromation retrevial:
    - first_set_bit / clear_bit
    - last_set_bit / clear_bit
*/

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
    uint8_t data[];
};

BitArray* BitArray_init(index_t size)
{
    BitArray* bit_array = calloc(sizeof(*bit_array) + BYTES_FROM_BITS(size), 1);
    if (bit_array == NULL) {
        fprintf(stderr, "Error allocating space for the BitArray of size %zu bits: %s\n", size, strerror(errno));
        return NULL;
    }
    bit_array->num_bits = size;

    return bit_array;
}

// No prefixes
BitArray* internal_BitArray_init_from_str(const char* str, NumType format)
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
        else if (('A' <= (val = toupper(val))) && val <= 'F' && (format == HEXADECIMAL))
            val = 10 + (val - 'A');
        else {
            fprintf(stderr, "Invalid %s string.\n", format == HEXADECIMAL ? "hexadecimal" : "decimal");
            exit(EXIT_FAILURE);
        }

        // Sets upper / lower nibbles of each byte
        bit_array->data[(i - 1) / 2] |= val << ((i & 1) * 4);
    }

    return bit_array;
}

inline BitArray* BitArray_init_from_hex(const char* str)
{
    return internal_BitArray_init_from_str(str, HEXADECIMAL);
}

inline BitArray* BitArray_init_from_dec(const char* str)
{
    return internal_BitArray_init_from_str(str, DECIMAL);
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

inline void BitArray_free(BitArray* bit_array)
{
    free(bit_array);
}

BitArray* BitArray_resize(BitArray* bit_array, index_t size)
{
    BitArray* tmp = realloc(bit_array, sizeof(*bit_array) + BYTES_FROM_BITS(size));
    if (tmp == NULL) {
        fprintf(stderr, "Unable to resize BitArray to size %zu bits: %s\n", size, strerror(errno));
        return NULL;
    }
    tmp->num_bits = size;

    if (size > tmp->num_bits)
        BitArray_clear_region(bit_array, tmp->num_bits, size - 1);

    return tmp;
}

BitArray* BitArray_copy(const BitArray* bit_array)
{
    index_t size = sizeof(*bit_array) + BYTES_FROM_BITS(bit_array->num_bits);

    BitArray* new_bit_array = BitArray_init(size);
    memcpy(new_bit_array, bit_array, size);

    return new_bit_array;
}

BitState BitArray_check_bit(const BitArray* bit_array, index_t bit_index)
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

void internal_BitArray_modify_bits(BitArray* bit_array,
                                   void (*bit_operation)(BitArray*, index_t),
                                   index_t num_bits_to_set, ...)
{
    va_list arg_list;
    va_start(arg_list, num_bits_to_set);

    for (index_t i = 0; i < num_bits_to_set; ++i) {
        index_t index = va_arg(arg_list, index_t);
        bit_operation(bit_array, index);
    }
    va_end(arg_list);
}

// Helper method to apply a bit_operation to a region of the array
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
        for (uint8_t i = end_bit_offset; i >= 0; --i)
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
    memset(bit_array->data, SET_BYTE, BYTES_FROM_BITS(bit_array->num_bits));
}

inline void BitArray_clear(BitArray* bit_array)
{
    memset(bit_array->data, CLEAR_BYTE, BYTES_FROM_BITS(bit_array->num_bits));
}

void BitArray_toggle(BitArray* bit_array)
{
    for (index_t i = 0; i < BYTES_FROM_BITS(bit_array->num_bits); ++i)
        bit_array->data[i] ^= SET_BYTE;
}

inline index_t BitArray_size(const BitArray* bit_array) {
    return bit_array->num_bits;
}

index_t BitArray_num_set_bits(const BitArray* bit_array)
{
    index_t num_full_bytes = BYTES_FROM_BITS(bit_array->num_bits) - 1;
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

index_t internal_BitARray_find_bit(const BitArray* bit_array,
                                          index_t initial_index,
                                          BitState find_bit_state,
                                          SearchDirection search_direction)
{
    assert(initial_index < bit_array->num_bits && "Bit index is out of range");

    if ((initial_index == 0 && search_direction == SEARCH_BACKWARD) ||
        (initial_index == bit_array->num_bits - 1 && search_direction == SEARCH_FORWARD))
        return initial_index;

    index_t curr_index = initial_index + search_direction;
    while (curr_index > 0 && curr_index < bit_array->num_bits) {
        if (BitArray_check_bit(bit_array, curr_index) == find_bit_state)
            return curr_index;

        curr_index += search_direction;
    }

    // Check the 0th index seperatly if going backwards
    if (search_direction == SEARCH_BACKWARD &&
        BitArray_check_bit(bit_array, 0) == find_bit_state)
        return 0;

    return initial_index;
}

inline index_t BitArray_num_clear_bits(const BitArray* bit_array)
{
    return bit_array->num_bits - BitArray_num_set_bits(bit_array);
}

inline index_t BitArray_next_set_bit(const BitArray* bit_array, index_t initial_index)
{
    return internal_BitARray_find_bit(bit_array, initial_index, BIT_SET, SEARCH_FORWARD);
}

inline index_t BitArray_next_clear_bit(const BitArray* bit_array, index_t initial_index)
{
    return internal_BitARray_find_bit(bit_array, initial_index, BIT_CLEAR, SEARCH_FORWARD);
}

inline index_t BitArray_prev_set_bit(const BitArray* bit_array, index_t initial_index)
{
    return internal_BitARray_find_bit(bit_array, initial_index, BIT_SET, SEARCH_BACKWARD);
}

inline index_t BitArray_prev_clear_bit(const BitArray* bit_array, index_t initial_index)
{
    return internal_BitARray_find_bit(bit_array, initial_index, BIT_CLEAR, SEARCH_BACKWARD);
}

void BitArray_print_hex(const BitArray* bit_array, FILE* file_stream, index_t chars_per_line)
{
    const index_t num_nibbles = POS_CEIL(bit_array->num_bits / 4.0);

    for (index_t i = 1; i <= num_nibbles; ++i) {
        uint8_t nibble = bit_array->data[(i - 1) / 2] & (0XF << ((i & 1) * 4));

        fprintf(file_stream, "%x", nibble >> (i & 1 ? 4: 0));
            if (i != num_nibbles)
                fputs(i % chars_per_line == 0 ? "\n" : ", ", file_stream);
    }
    fputc('\n', file_stream);
}

void BitArray_print_bin(const BitArray* bit_array, FILE* file_stream, index_t chars_per_line)
{   
    for (index_t i = 1; i <= bit_array->num_bits; ++i) {
        fputc(BitArray_check_bit(bit_array, i - 1) + '0', file_stream);
        if (i != bit_array->num_bits)
            fputs(i % chars_per_line == 0 ? "\n" : ", ", file_stream);
    }
    fputc('\n', file_stream);
}

bool BitArray_save(const BitArray* bit_array, const char file_name[])
{
    FILE* fp = fopen(file_name, "w+b");
    if (fp == NULL) {
        fprintf(stderr, "Error saving BitArray '%s': %s\n", file_name, strerror(errno));
        return false;
    }

    bool result = true;
    const index_t num_bytes = sizeof(*bit_array) + BYTES_FROM_BITS(bit_array->num_bits);

    if ((fwrite(FILE_HEADER, sizeof(char), HEADER_LEN, fp) != HEADER_LEN) ||
        (fwrite(bit_array, 1, num_bytes, fp) != num_bytes)) {
            fprintf(stderr, "Error saving BitArray '%s': %s,\n", file_name, strerror(errno));
            result = false;
    }

    if (fclose(fp) == EOF) {
        fprintf(stderr, "Error saving BitArray '%s': %s\n", file_name, strerror(errno));
        result = false;
    }

    return result;
}

BitArray* BitArray_load(const char file_name[])
{
    char file_header[HEADER_LEN + 1] = {[HEADER_LEN] = '\0'};

    FILE* fp = fopen(file_name, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Unable to open '%s': %s\n", file_name, strerror(errno));
        return NULL;
    }

    if ((fread(file_header, sizeof(char), HEADER_LEN, fp) != HEADER_LEN) ||
        (strcmp(file_header, FILE_HEADER) != 0)) {
        (void)fclose(fp);
        return NULL;
    }

    index_t num_bits;
    if (fread(&num_bits, sizeof(index_t), 1, fp) != 1) {
        (void)fclose(fp);
        return NULL;
    }

    BitArray* bit_array = BitArray_init(num_bits);
    if (bit_array == NULL) {
        (void)fclose(fp);
        return NULL;
    }

    index_t data_bytes = BYTES_FROM_BITS(num_bits);
    if (fread(bit_array->data, 1, data_bytes, fp) != data_bytes) {
        (void)fclose(fp);
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