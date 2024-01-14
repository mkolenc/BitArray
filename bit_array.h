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



#endif /* BIT_ARRAY_IMPLEMENTATION */