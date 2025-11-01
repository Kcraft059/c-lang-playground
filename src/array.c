#include <array.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/**
 * Array lib implementation
 * Warning :
 * This lib doesn't do any check for data integrity, it assumes the user uses it right.
 * Passing wrong pointers which aren't a dynamic array will result in UB.
 */

/// Private functions def
void* __array_get_header(void* self);
void* __array_resize(void* self, size_t new_size);

/// Public functions

void* __array_init(size_t item_size, size_t capacity, Allocator* a) {  // Initializes a dynamic array in memory
  void* self = NULL;                                                   // Array to NULL pointer in case of failure
  size_t size = item_size * capacity + sizeof(arrayHeader);
  arrayHeader* header = a->alloc(size);

  if (header) {  // Sets header metadata
    header->allocator = a;
    header->capacity = capacity;
    header->item_size = item_size;
    header->length = 0;
    self = header + 1;  // Set the pointer at the start of the array
  }

  return (void*)self;
}

int __array_append(void** self, void* value) {      // Adds an element to the end of the array
  arrayHeader* header = __array_get_header(*self);  // Fetch array header

  if (header->length >= header->capacity) {
    *self = __array_resize(*self, header->capacity *= ARRAY_INCREMENT_COEF);

    if (!*self) return 0;
    header = __array_get_header(*self);
  };

  // printf("Copy of array offset %ld (%ld items) on %ld bytes plage\n", header->length * header->item_size, header->length, header->item_size);  // Debug

  memcpy((char*)(*self) + header->length * header->item_size,
         value,
         header->item_size);  // Copies the value at specified pointer with item size
  header->length++;

  return 1;
}

int __array_pop(void** self) {                      // Deref element at last index
  arrayHeader* header = __array_get_header(*self);  // Get header metadata

  if (header->length <= 0) return 0;  // Can't pop empty array

  header->length--;

  // printf("New length for array is %ld\n", header->length);

  if (header->length * 2 <= header->capacity && header->length >= 1)  // If new length is 2 time smaller than capacity, shrink array unless lenght is <= 1 (to prevent 0 capacity multiplication)
    *self = __array_resize(*self, header->length);

  if (!self) return 0;  // If resize fails

  return 1;
}

int __array_add(void** self, size_t item_index, void* value) {  // Add element at index
  arrayHeader* header = __array_get_header(*self);
  if (!header || item_index > header->length) return 0;  // If NULL header or negative index, return error

  if (header->length >= header->capacity) {
    *self = __array_resize(*self, header->capacity *= ARRAY_INCREMENT_COEF);

    if (!*self) return 0;
    header = __array_get_header(*self);
  };

  // printf("Moving at index %ld from %ld, on %ld bytes \n", (item_index + 1) * header->item_size, item_index * header->item_size, header->length * header->item_size);

  memmove(
      (char*)(*self) + (item_index + 1) * header->item_size,
      (char*)(*self) + item_index * header->item_size,
      (header->length - item_index) * header->item_size);

  memcpy((char*)(*self) + item_index * header->item_size,
         value,
         header->item_size);  // Copies the value at specified pointer with item size
  header->length++;

  return 1;
}

int __array_remove(void** self, size_t item_index) {  // Remove element at index
  arrayHeader* header = __array_get_header(*self);
  if (!header || item_index > header->length) return 0;  // If NULL header or negative index, return error

  memmove(  // Moves mem to new index : overwritte
      (char*)(*self) + item_index * header->item_size,
      (char*)(*self) + (item_index + 1) * header->item_size,
      (header->length - (item_index + 1)) * header->item_size);

  header->length--;  // Reduce array length

  if ((header->length) * 2 <= header->capacity && header->length >= 1)  // If new length is 2 time smaller than capacity, shrink array unless lenght is <= 1 (to prevent 0 capacity multiplication)
    *self = __array_resize(*self, header->length);

  if (!*self) return 0;

  return 1;
}

int __array_merge(void** self, void* array_b) {  // Create a new array from two dynamic arrays self, array_b, inheriting array_a properties
  if (!(*self && array_b)) return 0;             // If any array is NULL, error

  arrayHeader* header = __array_get_header(*self);
  arrayHeader* header_b = __array_get_header(array_b);

  if (!(header && header_b)) return 0;                     // If any array is NULL, error
  if (header->item_size != header_b->item_size) return 0;  // Type mismatch

  size_t size = header->length + header_b->length - 1;  // Decrement since, when counting powers of two, 0 is considered as a number eg: 0->31
  int i;
  for (i = 0; size > 0; size >>= 1) ++i;  // Count powers of two in size (size >>= 1 -> size /= 2 - bit shifting)
  size = (size_t)(1 << i);                // Set size to upper power of two

  *self = __array_resize(*self, size);

  if (!*self) return 0;
  header = __array_get_header(*self);

  memcpy(  // Appends other array to the end of self
      (char*)(*self) + header->length * header->item_size,
      array_b,
      header_b->length * header->item_size);
  header->length += header_b->length;

  return 1;
}

size_t array_length(void* self) {  // Get length of array
  arrayHeader* header = __array_get_header(self);
  if (!header) return -1;  // If NULL header, return error

  return header->length;  // Return reported length
}

int array_delete(void* self) {  // Free array from memory
  arrayHeader* header = __array_get_header(self);
  header->allocator->free(header);  // Free allocated pointer since alloc starts at header
  return 1;
}

/// Private functions

void* __array_get_header(void* self) {           // Retrieves header pointer
  if (!self) return NULL;                        // Prevents use of NULL as input
  arrayHeader* header = (arrayHeader*)self - 1;  // Casts to arrayHeader to decrement by the right size

  return (void*)header;
}

void* __array_resize(void* self, size_t new_size) {
  printf("Resize of %p to %ld items\n", self, new_size);  // Debug

  if (!self) return NULL;                          // Prevents use of NULL as input
  arrayHeader* header = __array_get_header(self);  // Gets header info

  if (!header) return NULL;
  header = header->allocator->realloc(
      header, new_size * header->item_size + sizeof(arrayHeader));  // Reallocate memory

  if (!header) return NULL;
  header->capacity = new_size;

  self = header + 1;  // Point to the array
  return (void*)self;
}
