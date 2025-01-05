# BitArray - An Interface for a Bit Array

## Overview

**bit_array.c** provides a simple and efficient implementation of a bit array for C/C++. A bit array represents a sequence of bits where each bit can be individually manipulated and queried. This library offers various functions for initializing, managing, and performing operations on a bit array.

**Author:** Max Kolenc
**Date:** Jan, 2024

## Table of Contents

1. [Build](#build)
2. [Usage](#usage)
3. [Typedefs](#typedefs)
4. [Function Prototypes](#function-prototypes)
   - [Initialization](#initialization)
   - [Memory Management](#memory-management)
   - [Bit Operations](#bit-operations)
   - [Region Operations](#region-operations)
   - [Information Retrieval](#information-retrieval)
   - [Searching](#searching)
   - [String Operations](#string-operations)
   - [File Operations](#file-operations)

## Build

To build the library, use the following command:

`make`

Additionally, if you wish to build the library without bounds-checking for a potential speedup, use:

`make fast`

To build and run the tests, execute:

`make tests`

## Usage

1. Include the "bit_array.h" header file at the top of your code:

`#include "bit_array.h"`

2. Make sure to add the following to your compiler arguments:

```
BitArray_path=path/to/bit_array/
gcc ... -I$(BitArray_path) -L$(BitArray_path) -lbitarr
```

## Typedefs

- **BitArray:**
  - Struct representing the bit array.

- **index_t:**
  - Typedef for the index type used in the bit array.

## Function Prototypes

### Initialization

- **Initializes a BitArray with all bits set to zero**

    Returns a pointer to the newly initialized BitArray on success, otherwise NULL.
    ```c
    BitArray* BitArray_init(index_t size)
    ```

- **Initializes a BitArray from a hexadecimal string.**

    Each hexadecimal character represents 4 bits ('0' is 0000, '1' is 0001, ..., 'F' is 1111). Returns a pointer to the newly initialized BitArray on success, otherwise NULL.
    ```c
    BitArray* BitArray_init_from_hex(const char* str)
    ```

- **Initializes a BitArray from a binary string.**

    Each binary character represents 1 bit ('0' is 0 and '1' is 1). Returns a pointer to the newly initialized BitArray on success, otherwise NULL.
    ```c
    BitArray* BitArray_init_from_bin(const char* str)
    ```

### Memory Management

- **Deallocates memory used for a previously initialized BitArray.**
    ```c
    void BitArray_free(BitArray* bit_array)
    ```

- **Resizes the BitArray.**

    Returns a pointer to the resized BitArray on success, or NULL otherwise. If extended, zeros will be added.
    ```c
    BitArray* BitArray_resize(BitArray* bit_array, index_t size)
    ```

- **Produces a deep copy of the original BitArray.**

    Returns a pointer to the duplicate BitArray on success, or NULL otherwise. Pointer needs to be passed to BitArray_free() when done to avoid memory leaks.
    ```c
    BitArray* BitArray_copy(const BitArray* bit_array)
    ```

### Bit Operations

- **Checks whether a specific bit in a BitArray is set.** True for set (1), false for clear (0)
    ```c
    bool BitArray_check_bit(const BitArray* bit_array, index_t bit_index);
    ```

- **Set a specific bit in the BitArray.**
    ```c
    void BitArray_set_bit(BitArray* bit_array, index_t bit_index)
    ```

- **Clear a specific bit in the BitArray.**
    ```c
    void BitArray_clear_bit(BitArray* bit_array, index_t bit_index)
    ```

- **Toggle a specific bit in the BitArray.**
    ```c
    void BitArray_toggle_bit(BitArray* bit_array, index_t bit_index)
    ```

- **Set one or more bits in the bit_array.**
    ```c
    void BitArray_set_bits(BitArray* bit_array, index_t num_bits_to_set, ...)

    // e.g. set bits 2, 8, 13, 46:
    BitArray_set_bits(bit_array, 4, 2, 8, 13, 46);
    ```

- **Clear one or more bits in the bit_array.**
    ```c
    void BitArray_clear_bits(BitArray* bit_array, index_t num_bits_to_clear, ...)

    // e.g. clears bits 1, 200, 9:
    BitArray_clear_bits(bit_array, 3, 1, 200, 9);
    ```

- **Toggle one or more bits in the bit_array.**
    ```c
    void BitArray_toggle_bits(BitArray* bit_array, index_t num_bits_to_toggle, ...)

    // e.g. toggle bits 15, 23, 99, 57, 30:
    BitArray_toggle_bits(bit_array, 5, 15, 23, 99, 57, 30);
    ```

### Region Operations

- **Sets a region of the BitArray (inclusive).**
    ```c
    void BitArray_set_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index)
    ```

- **Clears a region of the BitArray (inclusive).**
    ```c
   void BitArray_clear_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index)
    ```

- **Toggles a region of the BitArray (inclusive).**
    ```c
    void BitArray_toggle_region(BitArray* bit_array, index_t start_bit_index, index_t end_bit_index)
    ```

- **Sets the entire BitArray.**
    ```c
    void BitArray_set(BitArray* bit_array)
    ```

- **Clears the entire BitArray.**
    ```c
    void BitArray_clear(BitArray* bit_array)
    ```

- **Toggles the entire BitArray.**
    ```c
    void BitArray_toggle(BitArray* bit_array)
    ```

### Information Retrieval

- **Get the size of the BitArray in bits.**
    ```c
    index_t BitArray_size(const BitArray* bit_array)
    ```

- **Get the number of set bits in the BitArray.**
    ```c
    index_t BitArray_num_set_bits(const BitArray* bit_array)
    ```

- **Get the number of clear bits in the BitArray.**
    ```c
    index_t BitArray_num_clear_bits(const BitArray* bit_array)
    ```

### Searching

   If the specified bit is found, returns true and saves the index of the bit in the 'result' variable. Otherwise, returns false and leaves 'result' unchanged. Note, the 'initial_index' is included in the search.

- **Get the index of the next set bit in the BitArray.**
    ```c
    bool BitArray_next_set_bit(const BitArray* bit_array, index_t initial_index, index_t* result)
    ```

- **Get the index of the next clear bit in the BitArray.**
    ```c
    bool BitArray_next_clear_bit(const BitArray* bit_array, index_t initial_index, index_t* result)
    ```

- **Get the index of the previous set bit in the BitArray.**
    ```c
    bool BitArray_prev_set_bit(const BitArray* bit_array, index_t initial_index, index_t* result)
    ```

- **Get the index of the previous clear bit in the BitArray.**
    ```c
    bool BitArray_prev_clear_bit(const BitArray* bit_array, index_t initial_index, index_t* result)
    ```

    If searching for the 'first' specified bit, start searching forward from index 0. Otherwise, search backwards from index BitArray_size() - 1.

- **Get the index of the first clear bit.**
    ```c
    bool BitArray_first_clear_bit(const BitArray* bit_array, index_t* result)
    ```

- **Get the index of the first set bit.**
    ```c
    bool BitArray_first_set_bit(const BitArray* bit_array, index_t* result)
    ```

- **Get the index of the last clear bit.**
    ```c
    bool BitArray_last_clear_bit(const BitArray* bit_array, index_t* result)
    ```

- **Get the index of the last set bit.**
    ```c
    bool BitArray_last_set_bit(const BitArray* bit_array, index_t* result)
    ```

### String Operations

- **Calculates the minimum length of the hexadecimal string required to represent the BitArray.**

    Meant to be used in conjunction with BitArray_to_hex_str().
    ```c
    index_t BitArray_min_hex_str_len(const BitArray* bit_array)
    ```

- **Calculates the minimum length of the binary string required to represent the BitArray.**

    Meant to be used in conjunction with BitArray_to_bin_str().
    ```c
    index_t BitArray_min_bin_str_len(const BitArray* bit_array)
    ```

- **Converts a BitArray to a hexadecimal string.**

    Each hexadecimal character represents 4 bits ('0' is 0000, '1' is 0001, ..., 'F' is 1111). If the size of the BitArray is not a multiple of 4, the last nibble is padded with zeros to the left. For instance, if the BitArray size is 7, like [1111 111], it converts to [1111 0111] (represented as the string "F7"). The function saves the hexadecimal representation of the bit array at 'dst', followed by a null terminator, and then 'dst' is returned.
    ```c
    char* BitArray_to_hex_str(const BitArray* bit_array, char* dst)
    ```

- **Converts a BitArray to a binary string.**

    Each binary character represents 1 bit ('0' is 0 and '1' is 1). The function saves the binary representation of the bit array at 'dst', followed by a null terminator, and then 'dst' is returned.
    ```c
    char* BitArray_to_bin_str(const BitArray* bit_array, char* dst)
    ```

### File Operations

- **Prints the BitArray in a hexadecimal format to a file stream.**

    ```c
    void BitArray_print_hex(const BitArray* bit_array, FILE* file_stream, index_t chars_per_line)
    ```

- **Prints the BitArray in a binary format to a file stream.**

    ```c
    void BitArray_print_bin(const BitArray* bit_array, FILE* file_stream, index_t chars_per_line)
    ```

- **Save the BitArray to a file.**

    Returns true if the saving operation was successful; otherwise, returns false. Ensure that the file_name parameter includes the file extension, and note that this function is intended for binary file operations.
    ```c
    bool BitArray_save(const BitArray* bit_array, const char file_name[])
    ```

- **Load a BitArray from a previously saved BitArray file.**

    Returns a pointer to the loaded BitArray on success, or NULL otherwise. Pointer needs to be passed to
    BitArray_free() when done to avoid memory leaks.
    ```c
   BitArray* BitArray_load(const char file_name[])
    ```