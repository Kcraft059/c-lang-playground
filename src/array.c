#include <definitions.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void* __array_init(size_t item_size, size_t capacity /*, Allocator *a */) {
  void* ptr = 0;
  size_t size = item_size * capacity + sizeof(ArrayHeader);
  ArrayHeader* h = malloc(size);

  if (h) {
    h->capacity = capacity;
    h->item_size = item_size;
    ptr = h + 1;  // (depends on the casting of h)
  }

  return ptr;
}

void __array_append(void** inArray, void* value) {
  ArrayHeader* h = (ArrayHeader*)(*inArray) - 1; // Get the real start of the DynamicArray

  h = realloc(h, (h->capacity + 1) * h->item_size + sizeof(ArrayHeader));
  if (h) {
    *inArray = h + 1;

    // since a char * is of size 1 byte
    memcpy((char*)(*inArray) + (h->capacity * h->item_size), value, h->item_size);
    ++h->capacity;
  } else {
    *inArray = NULL;
  }
}

int array_length(void* inArray) {
  ArrayHeader* h = (ArrayHeader*)inArray - 1;
  return h->capacity;
}