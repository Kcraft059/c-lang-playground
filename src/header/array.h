#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H
// Prevents from being included multiple times

#include <unistd.h>

/**
 * Dynamic array lib
 */

/// Default values

#define ARRAY_INITIAL_CAPACITY 1  // > 1
#define ARRAY_INCREMENT_COEF 2    // > 2

/// Macros

#define array(type, alloc) \
  ((type*)__array_init(sizeof(type), ARRAY_INITIAL_CAPACITY, alloc))
#define array_append(array_ptr, value) \
  (__array_append((void**)array_ptr, value))
#define array_pop(array_ptr) \
  (__array_pop((void**)array_ptr))
#define array_add(array_ptr, item_index, value) \
  (__array_add((void**)array_ptr, item_index, value))
#define array_remove(array_ptr, item_index) \
  (__array_remove((void**)array_ptr, item_index))
#define array_merge(self, array_b) \
  (__array_merge((void**)self, array_b))

/// Type definition

// Allocator descriptor
typedef struct {
  void* (*alloc)(size_t size);
  void* (*realloc)(void* ptr, size_t size);
  void (*free)(void* ptr);
  // memmov & memcpy are part of string.h
} Allocator;

typedef struct {
  size_t length;         // Number of elements
  size_t capacity;       // Real size in memory
  size_t item_size;      // Size of each item
  Allocator* allocator;  // Used allocator
} arrayHeader;

/// Function definitions

void* __array_init(size_t item_size, size_t capacity, Allocator* a);  // Initializes a dynamic array in memory
int __array_append(void** self, void* value);                         // Adds an element to the end of the array
int __array_pop(void** self);                                         // Removes element at last index
int __array_add(void** self, size_t item_index, void* value);         // Add element at index
int __array_remove(void** self, size_t item_index);                   // Remove element at index
int __array_merge(void** array_a, void* array_b);                    // Create a new array from two dynamic arrays array_a, array_b
size_t array_length(void* self);                                      // Get length of array
int array_delete(void* self);                                         // Free array from memory

#endif