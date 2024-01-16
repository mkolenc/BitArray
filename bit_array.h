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

// Constants
enum BitState {
    BIT_CLEAR,
	BIT_SET
};

// typedefs
typedef struct BitArray BitArray;
typedef size_t index_t;
typedef enum BitState BitState;


// Initialization and Memory Management
// ----------------------------------------------------------------------------------------------------------

BitArray* BitArray_init(index_t size, BitState initial_bit_state);
void BitArray_free(BitArray* bit_array);
BitArray* BitArray_resize(BitArray* bit_array, index_t new_num_bits);
BitArray* BitArray_copy(const BitArray* old_bit_array);

// Bit Operations
// ----------------------------------------------------------------------------------------------------------

BitState BitArray_check_bit(const BitArray* bit_array, index_t bit_index);
void BitArray_set_bit(BitArray* bit_array, index_t bit_index);
void BitArray_clear_bit(BitArray* bit_array, index_t bit_index);
void BitArray_toggle_bit(BitArray* bit_array, index_t bit_index);

void BitArray_set_bits(BitArray* bit_array, index_t num_bits_to_set, ...);
void BitArray_clear_bits(BitArray* bit_array, index_t num_bits_to_clear, ...);
void BitArray_toggle_bits(BitArray* bit_array, index_t num_bits_to_toggle, ...);

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

void BitArray_print(const BitArray* bit_array, FILE* file_stream, index_t bits_per_line);
bool BitArray_save(const BitArray* bit_array, const char file_name[]);
BitArray* BitArray_load(const char file_name[]);

#endif  /* BIT_ARRAY_H */

//
//
////   end header file   /////////////////////////////////////////////////////

#ifdef BIT_ARRAY_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

// Constants
#define SET_BYTE ((unsigned char) ~0)
#define CLEAR_BYTE ((unsigned char) 0)

enum SearchDirection {
    SEARCH_BACKWARD = -1,
	SEARCH_FORWARD = 1
};

// Macro functions
#define BYTES_FROM_BITS(bits) (POSITIVE_CEIL(((long double) (bits)) / CHAR_BIT))
#define POSITIVE_CEIL(x) ((x) > ((index_t)(x)) ? ((index_t)(x) + 1) : ((index_t)(x))) 
#define BYTE_INDEX(index) ((index) / CHAR_BIT)
#define BIT_OFFSET(index) ((index) % CHAR_BIT)
#define GET_MASK(index) (1 << (CHAR_BIT - BIT_OFFSET(index) - 1)) // Generate a bit mask with a specific bit set for an unsigned char.
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

// Typedefs
typedef enum SearchDirection SearchDirection;

// Structure
struct BitArray {
    index_t num_bits;
	unsigned char data[];
};

BitArray* BitArray_init(index_t size, BitState initial_bit_state) 
{
    // Check for invalid parameters
    if ((initial_bit_state != BIT_SET) && (initial_bit_state != BIT_CLEAR)) {
        fprintf(stderr, "Invalid BitState. Expected BIT_SET or BIT_CLEAR\n");
        return NULL;
    }

    // Allocate memory for the BitArray structure
    index_t num_bytes = BYTES_FROM_BITS(size);
    BitArray* bit_array = (BitArray*) malloc(sizeof(*bit_array) + (num_bytes * sizeof(unsigned char)));
    if (bit_array == NULL) {
        fprintf(stderr, "Error allocating space for the BitArray with %zu bits: %s\n", size, strerror(errno));
        return NULL;
    }
    bit_array->num_bits = size;

    if (initial_bit_state == BIT_SET)
        memset(bit_array->data, SET_BYTE, num_bytes);
    else
        memset(bit_array->data, CLEAR_BYTE num_bytes);

    return bit_array;
}

inline void BitArray_free(BitArray* bit_array)
{
	free(bit_array);
}

BitArray* BitArray_resize(BitArray* bit_array, index_t new_num_bits)
{	
	// Attempt to resize the BitArray by reallocating memory
	unsigned char* tmp = (unsigned char*) realloc(bit_array, sizeof(*bit_array) + (BYTES_FROM_BITS(new_num_bits) * sizeof(unsigned char)));
	if (tmp == NULL) {
        fprintf(stderr, "Unable to resize BitArray to size %zu: %zu\n", new_num_bits, strerror(errno));
        return NULL;
    }
    bit_array = tmp;
	
	// If the resizing extends the BitArray, set the new region to zero
	if (new_num_bits > bit_array->num_bits)
		BitArray_clear_region(bit_array, bit_array->num_bits, new_num_bits - 1);

	bit_array->num_bits = new_num_bits;
	return bit_array;
}

BitArray* BitArray_copy(const BitArray* old_BitArray) 
{
    // Calculate the size in bytes of the BitArray
    index_t size = sizeof(*old_BitArray) + (BYTES_FROM_BITS(old_BitArray->num_bits) * sizeof(unsigned char));

    // Allocate memory for the duplicate BitArray
    BitArray* new_BitArray = (BitArray*) malloc(size);
    if (new_BitArray == NULL) {
        fprintf(stderr, "Error allocating space for the duplicate BitArray: %s\n", strerror(errno));
        return NULL;
    }

    // Copy the content from the old BitArray to the new BitArray
    memcpy(new_BitArray, old_BitArray, size);

    return new_BitArray;
}

// Private helper function to validate we can preform bit operations
static void internal_BitArray_validate_index(const BitArray* bit_array, index_t index)
{
	if (index >= bit_array->num_bits) {
        fprintf(stderr, "Error: Index %zu is out of range for BitArray of size %zu\n", index, bit_array->num_bits);
        exit(EXIT_FAILURE);
    }
}

BitState BitArray_check_bit(const BitArray* bit_array, index_t bit_index)
{	
	internal_BitArray_validate_index(bit_array, bit_index);
	return (bit_array->data[BYTE_INDEX(bit_index)] & GET_MASK(bit_index)) == BIT_CLEAR ? BIT_CLEAR : BIT_SET;
}

void BitArray_set_bit(BitArray* bit_array, index_t bit_index)
{
	internal_BitArray_validate_index(bit_array, bit_index);
	bit_array->data[BYTE_INDEX(bit_index)] |= GET_MASK(bit_index);
}

void BitArray_clear_bit(BitArray* bit_array, index_t bit_index)
{
	internal_BitArray_validate_index(bit_array, bit_index);
	bit_array->data[BYTE_INDEX(bit_index)] &= ~GET_MASK(bit_index);
}

void BitArray_toggle_bit(BitArray* bit_array, index_t bit_index)
{
	internal_BitArray_validate_index(bit_array, bit_index);
	bit_array->data[BYTE_INDEX(bit_index)] ^= GET_MASK(bit_index);
}

// Private helper function to modify specific bits in one function call
void internal_BitArray_modify_bits(BitArray* bit_array,
                                   void (*bit_operation)(BitArray*, index_t), 
                                   index_t num_bits_to_set, ...)
{
    va_list arg_list;
    va_start(arg_list, num_bits_to_set);

	for (index_t i = 0; i < num_bits_to_set; i++) {
		index_t index = va_arg(arg_list, index_t);
		bit_operation(bit_array, index);
	}
	va_end(arg_list);
}

inline void BitArray_set_bits(BitArray* bit_array, index_t num_bits_to_set, ...)
{
    internal_BitArray_modify_bits(bit_array, BitArray_set_bit, num_bits_to_set, ...);
}

inline void BitArray_clear_bits(BitArray* bit_array, index_t num_bits_to_clear, ...)
{
    internal_BitArray_modify_bits(bit_array, BitArray_clear_bit, num_bits_to_clear, ...);
}

inline void BitArray_toggle_bits(BitArray* bit_array, index_t num_bits_to_toggle, ...)
{
    internal_BitArray_modify_bits(bit_array, BitArray_toggle_bit, num_bits_to_toggle, ...);
}

// Private helper function to operate on a region of the BitArray
// bit_operation is a function pointer to either BitArray_clear_bit, BitArray_set_bit or BitArray_toggle_bit.
static void internal_BitArray_operate_region(BitArray* bit_array, 
                                             index_t start_bit_index, 
                                             index_t end_bit_index, 
                                             void (*bit_operation)(BitArray*, index_t))
{
    internal_BitArray_validate_index(bit_array, start_bit_index);
    internal_BitArray_validate_index(bit_array, end_bit_index);

	start_bit_index = MIN(start_bit_index, end_bit_index);
	end_bit_index = MAX(start_bit_index, end_bit_index);

    index_t start_byte = BYTE_INDEX(start_bit_index);
    index_t end_byte = BYTE_INDEX(end_bit_index);
	
	// Operate on bits within the same byte
	if ((start_byte == end_byte)) {
		for (index_t i = start_bit_index; i <= end_bit_index; i++)
			bit_operation(bit_array, index);
		return;
	}

    // Operate on bits in the first byte
    char start_bit_offset = BIT_OFFSET(start_bit_index);
    if (start_bit_offset != 0) {
        for (char i = start_bit_offset; i < CHAR_BIT; i++)
			bit_operation(bit_array, start_bit_index++);
        start_byte++;
    }

    // Operate on bits in the last byte
    char end_bit_offset = BIT_OFFSET(end_bit_index);
    if (end_bit_offset != CHAR_BIT - 1) {
        for (char i = end_bit_offset; i >= 0; i--) 
			bit_operation(bit_array, end_bit_index--);
        end_byte--;
    }

	// Operate on the remaining bytes 
	unsigned char* ptr = bit_array->data + start_byte;
    index_t remaining_bytes = end_byte - start_byte + 1;

    if (bit_operation == BitArray_clear_bit)
		memset(ptr, CLEAR_BYTE, remaining_bytes);
    else if (bit_operation == BitArray_set_bit)
		memset(ptr, SET_BYTE, remaining_bytes);
    else
		for (index_t i = 0; i < remaining_bytes; i++)
			ptr[i] ^= SET_BYTE; // toggles a byte
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
	index_t byte_size = BYTES_FROM_BITS(bit_array->num_bits);
	for (index_t i = 0; i < byte_size; i++)
		bit_array->data[i] ^= SET_BYTE;
}

inline index_t BitArray_size(const BitArray* bit_array) {
    return (bit_array->num_bits);
}

index_t BitArray_num_set_bits(const BitArray* bit_array) 
{
    index_t num_full_bytes = BYTES_FROM_BITS(bit_array->num_bits) - 1;
    index_t BitArray_num_set_bits = 0;

    for (index_t i = 0; i < num_full_bytes; i++) {
		// Used to map an unsigned char to the number of 'ON' bits
		static const unsigned char bits_set_count_table[SET_BYTE + 1] = {
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
	index_t curr_bit_index = num_full_bytes * CHAR_BIT;
	index_t remaining_bits = bit_array->num_bits - curr_bit_index;

	for (index_t i = 0; i < remaining_bits; i++)
		if (BitArray_check_bit(bit_array, curr_bit_index++) == BIT_SET)
			BitArray_num_set_bits++;

    return BitArray_num_set_bits;
}

// Private helper function to search for a particular bit
static index_t internal_BitARray_find_bit(const BitArray* bit_array,
                                          index_t initial_index, 
                                          BitState find_bit_state, 
                                          SearchDirection search_direction)
{
	internal_BitArray_validate_index(bit_array, initial_index);
 
    if ((initial_index == 0 && search_direction == SEARCH_BACKWARD) ||
        (initial_index == bit_array->num_bits - 1 && search_direction == SEARCH_FORWARD))
        return initial_index;
    
    index_t curr_index = initial_index + search_direction;
    while (curr_index > 0 && curr_index < bit_array->num_bits) {
        BitState curr_bit_state = BitArray_check_bit(bit_array, curr_index);
        if (curr_bit_state == find_bit_state)
            return curr_index;
        curr_index += search_direction;
    }

    // Check the 0th index if going backwards
    if (search_direction == SEARCH_BACKWARD && BitArray_check_bit(bit_array, 0) == find_bit_state)
        return 0;

    return initial_index;
}

inline index_t BitArray_num_clear_bits(const BitArray* bit_array)
{
	return (bit_array->num_bits - BitArray_num_set_bits(bit_array));
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

#endif /* BIT_ARRAY_IMPLEMENTATION */