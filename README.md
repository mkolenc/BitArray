# BitArray - An Interface for a Bit Array

## Overview

**bit_array.h** provides a simple and efficient implementation of a bit array in C. A bit array represents a sequence of bits where each bit can be individually manipulated and queried. This header file offers various functions for initializing, managing, and performing operations on a bit array.

**Author:** Max Kolenc  
**Date:** Jan, 2024

## Table of Contents

1. [Constants and Typedefs](#constants-and-typedefs)
2. [Function Prototypes](#function-prototypes)
   - [Initialization and Memory Management](#initialization-and-memory-management)
   - [Bit Operations](#bit-operations)
   - [Region Operations](#region-operations)
   - [Information Retrieval](#information-retrieval)
   - [File Operations](#file-operations)
3. [Usage](#usage)

## Constants and Typedefs

- **BitState:**
  - Enum representing the possible states of a bit: `BIT_CLEAR` (0) and `BIT_SET` (1).

- **BitArray:**
  - Struct representing the bit array.
  
- **index_t:**
  - Typedef for the index type used in the bit array.

## Function Prototypes

### Initialization and Memory Management

- **Initialize a new BitArray.**

    Returns a pointer to the newly initialized BitArray on success, otherwise NULL.
    ```c
    BitArray* BitArray_init(index_t size, BitState initial_bit_state)
    ```

- **Deallocates memory used for a previously initialized BitArray.**
    ```c
    void BitArray_free(BitArray* bit_array)
    ```

- **Resizes the BitArray.**

    Returns a pointer to the resized BitArray on success, or NULL otherwise. If extended, zeros will be added.
    ```c
    BitArray* BitArray_resize(BitArray* bit_array, index_t new_num_bits)
    ```

- **Produces a deep copy of the original BitArray.**

    Returns a pointer to the duplicate BitArray on success, or NULL otherwise. Pointer needs to be passed to BitArray_free() when done to avoid memory leaks.
    ```c
    BitArray* BitArray_copy(const BitArray* old_bit_array)
    ```

### Bit Operations

- **Determine the state of a specific bit.**
    ```c
    BitState BitArray_check_bit(const BitArray* bit_array, index_t bit_index)
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
    BitArray_set_bits(bit_array, num_bits_to_set, ...)
    
    // e.g. set bits 2, 8, 13, 46:
    BitArray_set_bits(bit_array, 4, 2, 8, 13, 46);
    ```

- **Clear one or more bits in the bit_array.**
    ```c
    BitArray_clear_bits(bit_array, num_bits_to_set, ...)
    
    // e.g. clears bits 1, 200, 9:
    BitArray_clear_bits(bit_array, 3, 1, 200, 9);
    ```

- **Toggle one or more bits in the bit_array.**
    ```c
    BitArray_toggle_bits(bit_array, num_bits_to_set, ...)
    
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

- **Get the size of the BitArray.**
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
    
- **Get the index of the next set bit in the BitArray.**

    Returns the same initial_index if no set bits were found.
    ```c
    index_t BitArray_next_set_bit(const BitArray* bit_array, index_t initial_index)
    ```

- **Get the index of the next clear bit in the BitArray.**

    Returns the same initial_index if no clear bits were found.
    ```c
    index_t BitArray_next_clear_bit(const BitArray* bit_array, index_t initial_index)
    ```

- **Get the index of the previous set bit in the BitArray.**

    Returns the same initial_index if no set bits were found.
    ```c
    index_t BitArray_prev_set_bit(const BitArray* bit_array, index_t initial_index)
    ```

- **Get the index of the previous clear bit in the BitArray.**

    Returns the same initial_index if no clear bits were found.
    ```c
    index_t BitArray_prev_clear_bit(const BitArray* bit_array, index_t initial_index)
    ```

### File Operations

- **Prints the BitArray in a readable format to a file stream.**
    ```c
    void BitArray_print(const BitArray* bit_array, FILE* file_stream, index_t bits_per_line)
    ```

- **Save the BitArray to a file.**

    Returns true if the saving operation was successful; otherwise, returns false. Ensure that the file_name parameter includes the
    file extension, and note that this function is intended for binary file operations.
    ```c
    bool BitArray_save(const BitArray* bit_array, const char file_name[])
    ```

- **Load a BitArray from a previously saved BitArray file.**

    Returns a pointer to the loaded BitArray on success, or NULL otherwise. Pointer needs to be passed to
    BitArray_free() when done to avoid memory leaks.
    ```c
    BitArray* BitArray_load(const char file_name[])
    ```

## Usage

**bit_array.h** has been designed for ease of use, drawing inspiration from the stb libraries style. To seamlessly integrate it into your project, simply include the header directly into the files requiring its functionality. To create the implementaiton, define the `BIT_ARRAY_IMPLEMENTATION` macro before you include **bit_array.h** in **\*one\*** C or C++ file
(typically the main file).
```c
* #include ...
* #include ...
* #include ...
* #define BIT_ARRAY_IMPLEMENTATION
* #include "bit_array.h"

int main() {
    // Example usage
    BitArray* myBitArray = init_BitArray(32, BIT_CLEAR);
    set_bit(myBitArray, 5);
    // ...

    free_BitArray(myBitArray);
    return 0;
}
```