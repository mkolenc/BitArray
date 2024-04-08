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

// Uncomment the following line to disable bounds checking for bit operations
// #define BIT_ARRAY_FAST

// typedefs
typedef struct BitArray BitArray;
typedef size_t index_t;

// Initialization
// ----------------------------------------------------------------------------------------------------------

/*
 * BitArray_init: Initializes a BitArray with all bits set to zero.
 *
 * Parameters:
 *  - size: The number of bits in the BitArray.
 *
 * Returns:
 *  - A pointer to the initialized BitArray if successful, or NULL if memory allocation fails.
 */
BitArray* BitArray_init(index_t size);

/*
 * BitArray_init_from_hex: Initializes a BitArray from a hexadecimal string.
 *                         Each hexadecimal character represents 4 bits ('0' is 0000, '1' is 0001, ..., 'F' is 1111).
 *
 * Parameters:
 *  - str: A null-terminated string containing hexadecimal characters.
 *
 * Returns:
 *  - A pointer to the initialized BitArray if successful.
 *  - NULL if memory allocation fails or non-hexadecimal digits are encountered.
 */
BitArray* BitArray_init_from_hex(const char* str);

/*
 * BitArray_init_from_bin: Initializes a BitArray from a binary string.
 *                         Each binary character represents 1 bit ('0' is 0 and '1' is 1).
 *
 * Parameters:
 *  - str: A null-terminated string containing binary characters.
 *
 * Returns:
 *  - A pointer to the initialized BitArray if successful.
 *  - NULL if memory allocation fails or non-binary digits are encountered.
 */
BitArray* BitArray_init_from_bin(const char* str);

// Memory Management
// ----------------------------------------------------------------------------------------------------------

/*
 * BitArray_free: Frees the memory allocated for a BitArray.
 *
 * Parameters:
 *  - bit_array: A pointer to the BitArray to be freed.
 */
void BitArray_free(BitArray* bit_array);

/*
 * BitArray_resize: Resizes a BitArray to the specified size.
 *
 * Parameters:
 *  - bit_array: A pointer to the BitArray to be resized.
 *  - size: The new size of the BitArray in bits.
 *
 * Returns:
 *  - A pointer to the resized BitArray if successful.
 *  - NULL if memory allocation fails or if the specified size is zero.
 *
 * Remarks:
 *  - Additional memory is cleared when increasing the size.
 *  - Reallocating to a size of 0 is disallowed and will result in NULL being returned.
 */
BitArray* BitArray_resize(BitArray* bit_array, index_t size);

/*
 * BitArray_copy: Creates a deep copy of a BitArray.
 *
 * Parameters:
 *  - bit_array: A pointer to the BitArray to be copied.
 *
 * Returns:
 *  - A pointer to the newly created copy of the BitArray if successful.
 *  - NULL if memory allocation fails.
 *
 * Remarks:
 *  - Returned pointer needs to be passed to BitArray_free() when done to avoid memory leaks.
 */
BitArray* BitArray_copy(const BitArray* bit_array);

// Bit Operations
// ----------------------------------------------------------------------------------------------------------
//  - By default, these functions check whether the index passed is valid (In the range [0 to size - 1] inclusive).
//    An out-of-bounds check is considered critical enough to crash the program if violated.
//  - However, if you are confident in your code and wish to optimize for speed, you may uncomment the BIT_ARRAY_FAST
//    definition at the top of this header. This disables the error handling for bit operations.

/*
 * BitArray_check_bit: Checks whether a specific bit in a BitArray is set.
 *
 * Parameters:
 *  - bit_array: A pointer to the BitArray.
 *  - bit_index: The index of the bit to be checked.
 *
 * Returns:
 *  - True if the specified bit is set (1).
 *  - False if the specified bit is not set (0).
 */
bool BitArray_check_bit(const BitArray* bit_array, index_t bit_index);

/*
 * Set, clear or toggle (0 -> 1, 1 -> 0) a specific bit in a BitArray.
 *
 * Parameters:
 *  - bit_array: A pointer to the BitArray.
 *  - bit_index: The index of the bit to be set/cleared/toggled.
 */
void BitArray_set_bit(BitArray* bit_array, index_t bit_index);
void BitArray_clear_bit(BitArray* bit_array, index_t bit_index);
void BitArray_toggle_bit(BitArray* bit_array, index_t bit_index);

/*
 * Set, clear or toggle (0 -> 1, 1 -> 0) multiple bits in a BitArray.
 *
 * Parameters:
 *  - bit_array:                    A pointer to the BitArray.
 *  - num_bits_to_set/clear/toggle: The number of bits you wish to modify.
 *  - Additional parameters:        The indicies of the bits you wish to modify.
 *
 * Preconditions:
 *  - num_bits_to_set/clear/toggle is correct
 *  - Atleast one additional parameter / index is passed.
 */
void BitArray_set_bits(BitArray* bit_array, index_t num_bits_to_set, ...);
void BitArray_clear_bits(BitArray* bit_array, index_t num_bits_to_clear, ...);
void BitArray_toggle_bits(BitArray* bit_array, index_t num_bits_to_toggle, ...);

// Region Operations
// ----------------------------------------------------------------------------------------------------------

/*
 * Sets, clears or toggles a region of bits within a BitArray from start_bit_index to end_bit_index inclusive.
 *
 * Parameters:
 *  - bit_array: Pointer to the BitArray.
 *  - start_bit_index: Index of the first bit in the region to be set.
 *  - end_bit_index: Index of the last bit in the region to be set.
 */
void BitArray_set_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index);
void BitArray_clear_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index);
void BitArray_toggle_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index);

/*
 * Sets, clears or toggles the entier BitArray.
 *
 * Parameters:
 *  - bit_array: Pointer to the BitArray.
 */
void BitArray_set(BitArray* bit_array);
void BitArray_clear(BitArray* bit_array);
void BitArray_toggle(BitArray* bit_array);

// Information Retrieval
// ----------------------------------------------------------------------------------------------------------

/*
 * BitArray_size: Returns the size of the BitArray in bits.
 *
 * Parameters:
 *  - bit_array: Pointer to the BitArray.
 *
 * Returns:
 *  - The size of the BitArray in bits.
 */
index_t BitArray_size(const BitArray* bit_array);

/*
 * Returns the number of set/clear bits in the BitArray.
 *
 * Parameters:
 *  - bit_array: Pointer to the BitArray.
 *
 * Returns:
 *  - The number of set/clear bits in the BitArray.
 */
index_t BitArray_num_set_bits(const BitArray* bit_array);
index_t BitArray_num_clear_bits(const BitArray* bit_array);

/*
 * Searches for the next or previous set/clear bit in the BitArray from a given index.
 *
 * Parameters:
 *  - bit_array: Pointer to the BitArray to search.
 *  - initial_index: Starting index for the search.
 *  - result: Pointer to store the index of the found bit (if any).
 *
 * Returns:
 *  - True if the specified bit was found, false otherwise.
 *
 * Remarks:
 *  - The search includes the initial_index.
 *  - Use BitArray_next_set_bit/BitArray_next_clear_bit for forward search,
 *    BitArray_prev_set_bit/BitArray_prev_clear_bit for backward search.
 *  - If the specified bit is not found, the result is not modified.
 */
bool BitArray_next_set_bit(const BitArray* bit_array, index_t initial_index, index_t* result);
bool BitArray_next_clear_bit(const BitArray* bit_array, index_t initial_index, index_t* result);
bool BitArray_prev_set_bit(const BitArray* bit_array, index_t initial_index, index_t* result);
bool BitArray_prev_clear_bit(const BitArray* bit_array, index_t initial_index, index_t* result);

// Same as above
// Starts at initial_index 0 if searching for the 'first', or size - 1 if searching for the 'last'
bool BitArray_first_clear_bit(const BitArray* bit_array, index_t* result);
bool BitArray_first_set_bit(const BitArray* bit_array, index_t* result);
bool BitArray_last_clear_bit(const BitArray* bit_array, index_t* result);
bool BitArray_last_set_bit(const BitArray* bit_array, index_t* result);

// String functions
// ----------------------------------------------------------------------------------------------------------

/*
 * Calculates the minimum length of the hexadecimal/binary string required to represent the BitArray.
 * These functions are meant to be used in conjunction with BitArray_to_hex_str() and BitArray_to_bin_str().
 *
 * Parameters:
 *  - bit_array: Pointer to the BitArray.
 *
 * Returns:
 *  - The minimum length of characters required to represent the BitArray.
 */
index_t BitArray_min_hex_str_len(const BitArray* bit_array);
index_t BitArray_min_bin_str_len(const BitArray* bit_array);

/*
 * BitArray_to_hex_str: Converts a BitArray to a hexadecimal string.
 *                      Each hexadecimal character represents 4 bits ('0' is 0000, '1' is 0001, ..., 'F' is 1111).
 *                      In cases where the size of the BitArray is not a multiple of 4,
 *                      the last nibble is forward-padded with zeros.
 *                      For example, a BitArray of size 7: [1111 111] is converted to the string "F7".
 *
 * Preconditions:
 *  - dst must be non-NULL and have sufficient space to store the hexadecimal representation of the BitArray,
 *    you can determine this length by calling BitArray_min_hex_str_len().
 *
 * Parameters:
 *  - dst: A pointer to the destination where the hexadecimal characters will be stored.
 *
 * Returns:
 *  - The same pointer dst on success, or NULL otherwise.
 *
 * Remarks:
 *  - dst is guaranteed to be a valid string as a null terminator is appended to the hex string.
 */
char* BitArray_to_hex_str(const BitArray* bit_array, char* dst);

/*
 * BitArray_to_bin_str: Converts a BitArray to a binary string.
 *                      Each binary character represents 1 bit ('0' is 0 and '1' is 1).
 *
 * Preconditions:
 *  - dst must be non-NULL and have sufficient space to store the binary representation of the BitArray,
 *    you can determine this length by calling BitArray_min_bin_str_len().
 *
 * Parameters:
 *  - dst: A pointer to the destination where the binary characters will be stored.
 *
 * Returns:
 *  - The same pointer dst on success, or NULL otherwise.
 *
 * Remarks:
 *  - dst is guaranteed to be a valid string as a null terminator is appended to the bin string.
 */
char* BitArray_to_bin_str(const BitArray* bit_array, char* dst);

// File Operations
// ----------------------------------------------------------------------------------------------------------

/*
 * Same as above but prints to a file stream instead of a string.
 *
 * Preconditions:
 *  - chars_pre_line is non-zero.
 */
void BitArray_print_hex(const BitArray* bit_array, FILE* file_stream, index_t chars_per_line);
void BitArray_print_bin(const BitArray* bit_array, FILE* file_stream, index_t chars_per_line);

/*
 * BitArray_save: Saves the BitArray to a file.
 *
 * Parameters:
 *  - bit_array: Pointer to the BitArray to be saved.
 *  - file_name: The name of the file to which the BitArray will be saved.
 *
 * Returns:
 *  - True if the BitArray was successfully saved to the file, false otherwise.
 *
 * Remarks:
 *  - The file is created if it does not exist, and overwritten if it already exists.
 */
bool BitArray_save(const BitArray* bit_array, const char file_name[]);

/*
 * BitArray_load: Loads a BitArray from a file.
 *
 * Parameters:
 *  - file_name: The name of the file from which to load the BitArray.
 *
 * Returns:
 *  - A pointer to the BitArray loaded from the file if successful, or NULL otherwise.
 *
 * Remarks:
 *  - The returned BitArray needs to be freed using BitArray_free() when no longer needed.
 */
BitArray* BitArray_load(const char file_name[]);

#endif  /* BIT_ARRAY_H */