/*
* bit_array.h -- An interface for working with bit arrays
*
* Author: Max Kolenc
* Date: Jan, 2024
*/
#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

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