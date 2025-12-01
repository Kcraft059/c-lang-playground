/**-----------------------------------------------------------------------*
 * ArrayLib by @kcraft059
 *
 * This personnal library implements array related concepts in c such as
 * hashmaps and dynamic arrays. The hashmap and dynamic array
 * implementations do not share any component.
 *
 * No copyright - Last updated: 11/11/2025
 *-----------------------------------------------------------------------**/

#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

/// Default values

#define ARRAY_INITIAL_CAPACITY 1 // > 1
#define ARRAY_INCREMENT_COEF 2   // > 2
// TODO Implement hysteresis to prevent realloc back and forth

/// Macros

#define array(type, alloc) \
  ((type*)__array_init(sizeof(type), ARRAY_INITIAL_CAPACITY, alloc))
#define array_dup(type, source) \
  ((type*)__array_duplicate(source))
#define array_resize(array_ptr, size) \
  (__array_resize((void**)array_ptr, size))
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

typedef struct Allocator Allocator;
typedef struct arrayHeader arrayHeader;

struct Allocator {
  void* (*alloc)(size_t size);
  void* (*realloc)(void* ptr, size_t size);
  void (*free)(void* ptr);
  // memmov & memcpy are part of string.h
};

struct arrayHeader {
  size_t length;        // Number of elements
  size_t capacity;      // Real size in memory
  size_t item_size;     // Size of each item
  Allocator* allocator; // Used allocator
};

/// Function definitions

void* __array_init(size_t item_size, size_t capacity, Allocator* a); // Initializes a dynamic array in memory
void* __array_duplicate(void* source);                               // Duplicates an array in memory
bool __array_resize(void** self, size_t size);                       // Change array capcity to number of items
size_t __array_append(void** self, void* value);                       // Adds an element to the end of the array
bool __array_pop(void** self);                                       // Removes element at last index
bool __array_add(void** self, size_t item_index, void* value);       // Add element at index
bool __array_remove(void** self, size_t item_index);                 // Remove element at index
bool __array_merge(void** array_a, void* array_b);                   // Create a new array from two dynamic arrays array_a, array_b
size_t array_length(void* self);                                     // Get length of array
void array_delete(void* self);                                       // Free array from memory

/**
 * Hash Map lib
 */

/// Default

#define HASHMAP_INITIAL_CAPACITY 16 // Should be equal to the total of items in the hashmap (>1)
#define HASHMAP_RESIZE_FACTOR 1.5   // If itemc > capacity * factor ++ - if itemc * factor < capacity
#define HASHMAP_RESIZE_COEF 2       // capacity * coef ++ - capacity / coef --

/// Macros

#define hashmap(preHashFunc, a) \
  (hashmap_init(HASHMAP_INITIAL_CAPACITY, preHashFunc, a))

/// Type definitions

typedef struct hashMap hashMap;
typedef struct bucketItem bucketItem;
typedef uint64_t hash;

typedef hash (*prehashFunc)(void* key);
typedef bool (*hashmapIterFunc)(bucketItem* self, void* extData); // Checks a condition on ptr, returns true to continue, false to stop iteration

struct bucketItem {        // Bucket item, which points to the data
  uint64_t cached_prehash; // Usefull when resizing hashmap
  void* ptr;
  bucketItem* nextItem; // Chain items in bucket
};

struct hashMap {           // Hash map for quick access given value
  size_t capacity;         // Bucket capacity
  size_t itemc;            // Stored items count
  bool dynamicResize;      // Should the hashmap be dynamically resized
  prehashFunc preHashFunc; // Generate a preHash for any given void* to be hashed
  Allocator* allc;
  bucketItem** bucketList; // Pointer to the memory adress of bucketlist
};

/// Function definitions
hashMap* hashmap_init(size_t initCapacity, hash (*preHashFunc)(void*), Allocator* a); // Inits the hashmap in memory
void hashmap_delete(hashMap* self);                                                   // Erase the hashmap in memory
void hashmap_add(hashMap* self, void* data, void* key);                               // Adds data for given key in hashmap
bool hashmap_remove(hashMap* self, void* key);                                        // Delete an item at given key in map
void* hashmap_get(hashMap* self, void* key);                                          // Get ptr to data for given key in hashmap
void hashmap_resize(hashMap* self, size_t capacity);                                  // Resize map to given capacity
bool hashmap_actOnEach(hashMap* self, hashmapIterFunc func, void* extData);           // Execute function on each element of the hashmap, returns false on failure

#endif
