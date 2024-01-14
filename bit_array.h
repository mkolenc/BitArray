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

// Macro functions
#define BYTES_FROM_BITS(bits) (POSITIVE_CEIL(((long double) (bits)) / CHAR_BIT))
#define POSITIVE_CEIL(x) ((x) > ((index_t)(x)) ? ((index_t)(x) + 1) : ((index_t)(x))) 

// Structure
struct BitArray {
    index_t num_bits;
	unsigned char data[];
};

BitArray* BitArray_init(index_t size, BitState initial_bit_state) 
{
    // Check for invalid parameters
    if (size == 0) {
        fprintf(stderr, "Can't create a BitArray of size 0\n");
        return NULL;
    }

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

void BitArray_free(BitArray* bit_array)
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
    size_t size = sizeof(*old_BitArray) + (BYTES_FROM_BITS(old_BitArray->num_bits) * sizeof(unsigned char));

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


#endif /* BIT_ARRAY_IMPLEMENTATION */