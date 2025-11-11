/**-----------------------------------------------------------------------*
 * ArrayLib by @kcraft059
 *
 * This personnal library implements array related concepts in c such as
 * hashmaps and dynamic arrays. The hashmap and dynamic array
 * implementations do not share any component.
 *
 * No copyright - Last updated: 11/11/2025
 *-----------------------------------------------------------------------**/

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
// Personnal lib
#include <array.h>

/**
 * Array lib implementation
 * Warning :
 * This lib doesn't do any check for data integrity, it assumes the user uses it right.
 * Passing wrong pointers which aren't a dynamic array will result in UB.
 */

/// Private functions def
// Static prevents to be seen by linker
static void* __array_get_header(void* self);
static void* __array_resize(void* self, size_t new_size);

/// Public functions

void* __array_init(size_t item_size, size_t capacity, Allocator* a) { // Initializes a dynamic array in memory
  void* self = NULL;                                                  // Array to NULL pointer in case of failure
  size_t size = item_size * capacity + sizeof(arrayHeader);
  arrayHeader* header = a->alloc(size);

  if (header) { // Sets header metadata
    header->allocator = a;
    header->capacity = capacity;
    header->item_size = item_size;
    header->length = 0;
    self = header + 1; // Set the pointer at the start of the array
  }

  return (void*)self;
}

void* __array_duplicate(void* source) { // Duplicates an array in memory
  if (!source) return NULL;
  arrayHeader* header = __array_get_header(source); // Get source header

  if (!header) return NULL;
  size_t size = header->item_size * header->capacity + sizeof(arrayHeader);
  void* self = header->allocator->alloc(size);

  memcpy(self, header, size);

  return (void*)((arrayHeader*)self + 1);
}

int __array_append(void** self, void* value) {     // Adds an element to the end of the array
  arrayHeader* header = __array_get_header(*self); // Fetch array header

  if (header->length >= header->capacity) {
    *self = __array_resize(*self, header->capacity *= ARRAY_INCREMENT_COEF);

    if (!*self) return 0;
    header = __array_get_header(*self);
  };

  // printf("Copy of array offset %ld (%ld items) on %ld bytes plage\n", header->length * header->item_size, header->length, header->item_size);  // Debug

  memcpy((char*)(*self) + header->length * header->item_size,
         value,
         header->item_size); // Copies the value at specified pointer with item size
  header->length++;

  return 1;
}

int __array_pop(void** self) {                     // Deref element at last index
  arrayHeader* header = __array_get_header(*self); // Get header metadata

  if (header->length <= 0) return 0; // Can't pop empty array

  header->length--;

  // printf("New length for array is %ld\n", header->length);

  if (header->length * 2 <= header->capacity && header->length >= 1) // If new length is 2 time smaller than capacity, shrink array unless lenght is <= 1 (to prevent 0 capacity multiplication)
    *self = __array_resize(*self, header->length);

  if (!*self) return 0; // If resize fails

  return 1;
}

int __array_add(void** self, size_t item_index, void* value) { // Add element at index
  arrayHeader* header = __array_get_header(*self);
  if (!header || item_index > header->length) return 0; // If NULL header or negative index, return error

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
         header->item_size); // Copies the value at specified pointer with item size
  header->length++;

  return 1;
}

int __array_remove(void** self, size_t item_index) { // Remove element at index
  arrayHeader* header = __array_get_header(*self);
  if (!header || item_index >= header->length) return 0; // If NULL header or negative index, return error

  memmove( // Moves mem to new index : overwritte
      (char*)(*self) + item_index * header->item_size,
      (char*)(*self) + (item_index + 1) * header->item_size,
      (header->length - (item_index + 1)) * header->item_size);

  header->length--; // Reduce array length

  if ((header->length) * 2 <= header->capacity && header->length >= 1) // If new length is 2 time smaller than capacity, shrink array unless lenght is <= 1 (to prevent 0 capacity multiplication)
    *self = __array_resize(*self, header->length);

  if (!*self) return 0;

  return 1;
}

int __array_merge(void** self, void* array_b) { // Create a new array from two dynamic arrays self, array_b, inheriting array_a properties
  if (!(*self && array_b)) return 0;            // If any array is NULL, error

  arrayHeader* header = __array_get_header(*self);
  arrayHeader* header_b = __array_get_header(array_b);

  if (!(header && header_b)) return 0;                    // If any array is NULL, error
  if (header->item_size != header_b->item_size) return 0; // Type mismatch

  size_t size = header->length + header_b->length - 1; // Decrement since, when counting powers of two, 0 is considered as a number eg: 0->31
  int i;
  for (i = 0; size > 0; size >>= 1)
    ++i;                   // Count powers of two in size (size >>= 1 -> size /= 2 - bit shifting)
  size = (size_t)(1 << i); // Set size to upper power of two

  *self = __array_resize(*self, size);

  if (!*self) return 0;
  header = __array_get_header(*self);

  memcpy( // Appends other array to the end of self
      (char*)(*self) + header->length * header->item_size,
      array_b,
      header_b->length * header->item_size);
  header->length += header_b->length;

  return 1;
}

size_t array_length(void* self) { // Get length of array
  arrayHeader* header = __array_get_header(self);
  if (!header) return -1; // If NULL header, return error

  return header->length; // Return reported length
}

int array_delete(void* self) { // Free array from memory
  arrayHeader* header = __array_get_header(self);
  header->allocator->free(header); // Free allocated pointer since alloc starts at header
  return 1;
}

/// Private functions

void* __array_get_header(void* self) {          // Retrieves header pointer
  if (!self) return NULL;                       // Prevents use of NULL as input
  arrayHeader* header = (arrayHeader*)self - 1; // Casts to arrayHeader to decrement by the right size

  return (void*)header;
}

void* __array_resize(void* self, size_t new_size) { // Resize size in memory∑
  // printf("Resize of %p to %ld items\n", self, new_size);  // Debug

  if (!self) return NULL;                         // Prevents use of NULL as input
  arrayHeader* header = __array_get_header(self); // Gets header info

  if (!header) return NULL;
  header = header->allocator->realloc(
      header, new_size * header->item_size + sizeof(arrayHeader)); // Reallocate memory

  if (!header) return NULL;
  header->capacity = new_size;

  self = header + 1; // Point to the array
  return (void*)self;
}

/**
 * Hash Map lib implementation
 * Warning :
 * This lib doesn't do any check for data integrity, it assumes the user uses it right.
 * Passing wrong pointers will result in UB.
 */

/// Private function declaration
static hash __hashmap_getpreHash(hashMap* self, void* key);    // Get hash for value
static size_t __hashmap_getIndex(hashMap* self, hash prehash); // Get index for hash
// static void __hashmap_resize(hashMap* self, size_t capacity);  // Resize & reassign hashmap buckets
static void __hashmap_bucketAdd(
    hashMap* targetMap, bucketItem** self, void* data, hash prehash); // Add & reassign bucket item with given prehash in bucket chain starting at self*
static bool __hashmap_bucketRemove(
    hashMap* targetMap, bucketItem** self, hash prehash); // Remove & relink bucket item with given prehash in bucket chain starting at self*
static void* __hashmap_bucketGet(
    bucketItem* self, hash prehash);                                                              // Get bucket item with given prehash in bucket chain starting at self
static void __hashmap_bucketChainReassign(bucketItem* self, hashMap* targetMap, Allocator* allc); // Reassign buckets starting at self, to targetMap (if NULL delete chain)
static inline hash __splitmix64(hash z);                                                          // Generate hash from given number (splitMix64 alg)

/// Function def

hashMap* hashmap_init(size_t initCapacity, hash (*preHashFunc)(void*), Allocator* a) { // Inits the hashmap in memory
  hashMap* self = a->alloc(sizeof(hashMap));                                           // Allocate memory for hashmap Header

  if (self) {
    self->capacity = initCapacity;                                   // Set capacity
    self->prehashFunc = preHashFunc;                                 // Sets the hasher function
    self->bucketList = a->alloc(sizeof(bucketItem*) * initCapacity); // Allocate memory for all buckets
    for (size_t i = 0; i < initCapacity; ++i) {                         // Set all pointers to NULL
      self->bucketList[i] = NULL;
    }
    self->itemc = 0;
    self->allc = a;
    self->dynamicResize = true;
  }

  return self;
}

void hashmap_delete(hashMap* self) { // Erase the hashmap in memory
  Allocator* a = self->allc;

  for (size_t i = 0; i < self->capacity; i++) { // For each bucket, free
    bucketItem* bucketChainStart = self->bucketList[i];
    if (bucketChainStart) {
      __hashmap_bucketChainReassign(bucketChainStart, NULL, self->allc);
    }
  }

  a->free(self->bucketList);
  a->free(self);
}

void hashmap_add(hashMap* self, void* data, void* key) {    // Adds data for given key in hashmap
  if (self->dynamicResize &&                                // If hashmap is dynamic
      self->itemc > self->capacity * HASHMAP_RESIZE_FACTOR) // If itemc is too high for capacity, resize up
    hashmap_resize(self, self->capacity * HASHMAP_RESIZE_COEF);

  hash prehash = __hashmap_getpreHash(self, key); // Should be after resize, because index depends on capacity
  size_t index = __hashmap_getIndex(self, prehash);

  bucketItem** bucket = self->bucketList + index;
  __hashmap_bucketAdd(self, bucket, data, prehash); // Add data in bucket
}

bool hashmap_remove(hashMap* self, void* key) {
  if (self->dynamicResize &&                                  // If hashmap is dynamic
      self->itemc * HASHMAP_RESIZE_FACTOR < self->capacity && // If itemc is too low for capacity, resize down
      self->capacity / HASHMAP_RESIZE_COEF > HASHMAP_RESIZE_COEF)
    hashmap_resize(self, self->capacity / HASHMAP_RESIZE_COEF);

  hash prehash = __hashmap_getpreHash(self, key); // Should be after resize, because index depends on capacity
  size_t index = __hashmap_getIndex(self, prehash);

  bucketItem** bucket = self->bucketList + index; // Starting pos for bucket chain

  return __hashmap_bucketRemove(self, bucket, prehash); // Remove bucket starting at bucket*
}

void* hashmap_get(hashMap* self, void* key) {
  hash prehash = __hashmap_getpreHash(self, key);   // Compute prehash
  size_t index = __hashmap_getIndex(self, prehash); // Get associated index

  bucketItem* bucket = self->bucketList[index];

  return __hashmap_bucketGet(bucket, prehash); // Check in bucket chain for matching hash
}

void hashmap_resize(hashMap* self, size_t capacity) { // Resize & reassign hashmap buckets
  bucketItem** oldMap = self->bucketList;
  size_t oldMapCapacity = self->capacity;

  self->bucketList = self->allc->alloc(sizeof(bucketItem*) * capacity); // Make a new bucket list
  for (int i = 0; i < capacity; ++i) {                                  // Set all pointers to NULL
    self->bucketList[i] = NULL;
  }

  self->capacity = capacity; // Set new capacity
  self->itemc = 0;           // Set to 0, since reassign uses bucket add which increments itemc

  for (size_t i = 0; i < oldMapCapacity; i++) { // For each bucket, reassign to new map
    bucketItem* bucketChainStart = oldMap[i];
    if (bucketChainStart) {
      __hashmap_bucketChainReassign(bucketChainStart, self, self->allc);
    }
  }

  self->allc->free(oldMap); // Free old map
}

/// Private function definition
// bucket handling
void __hashmap_bucketAdd(hashMap* targetMap, bucketItem** self, void* data, hash prehash) {
  if (!*self) {                                         // If self is not allocated, alloc it
    *self = targetMap->allc->alloc(sizeof(bucketItem)); // Init new bucket in memory
    (*self)->ptr = NULL;                                // Init values
    (*self)->nextItem = NULL;
    targetMap->itemc++; // Increase item count
  }

  if (!(*self)->ptr || (*self)->cached_prehash == prehash) { // If self ptr is NULL or If it's the same hash, replace data
    (*self)->cached_prehash = prehash;                       // Store hashkey
    (*self)->ptr = data;                                     // Store data
    return;
  }

  // else it is not null, then try to store in nextItem
  __hashmap_bucketAdd(targetMap, &(*self)->nextItem, data, prehash);
}

bool __hashmap_bucketRemove(hashMap* targetMap, bucketItem** self, hash prehash) {
  if (!*self) return false; // If targeted item points to nothing

  if ((*self)->cached_prehash == prehash) { // If targeted element is found
    bucketItem* next = (*self)->nextItem;
    targetMap->allc->free(*self); // Free bucket
    *self = next;                 // Set reference pointer to next item
    targetMap->itemc--;           // Decrease item count
    return true;
  }

  return __hashmap_bucketRemove(targetMap, &(*self)->nextItem, prehash); // If not found
}

void* __hashmap_bucketGet(bucketItem* self, hash prehash) {
  if (!self) return NULL; // If ptr passed si NULL

  if (self->cached_prehash == prehash) return self->ptr; // If matching prehash, return ptr of bucket

  return __hashmap_bucketGet(self->nextItem, prehash); // Else try for next item
}

void __hashmap_bucketChainReassign(bucketItem* self, hashMap* targetMap, Allocator* allc) { // Reassign buckets starting at self, to targetMap, if no targetMap delete all chain
  if (!self) return;                                                                        // If reached end of chain

  if (targetMap) {
    size_t index = __hashmap_getIndex(targetMap, self->cached_prehash); // Get index for newmap

    bucketItem** bucket = targetMap->bucketList + index;                     // Starting pos for bucket chain
    __hashmap_bucketAdd(targetMap, bucket, self->ptr, self->cached_prehash); // Transfer bucket content to new bucket chain
  }

  bucketItem* next = self->nextItem;
  allc->free(self); // Delete old bucket from memory

  __hashmap_bucketChainReassign(next, targetMap, allc);
}

// Hash index functions
size_t __hashmap_getIndex(hashMap* self, hash prehash) { // Get an index based on given hash key
  return __splitmix64(prehash) % self->capacity;         // Only calc hash for bucket index
}

hash __hashmap_getpreHash(hashMap* self, void* key) { // Get hash from key
  hash prehash = self->prehashFunc(key);
  return prehash;
}

hash __splitmix64(hash z) {   // Generate hash from given number (splitMix64 alg)
  z += 0x9e3779b97f4a7c15ULL; // z acting as seed
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
  z = z ^ (z >> 31);
  return z;
}