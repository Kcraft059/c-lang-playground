#include <unistd.h>

#define array(T) (T*)__array_init(sizeof(T), 0)
#define array_append(a, v) __array_append((void**)a, v)

// Type def

typedef struct {
  size_t capacity;
  size_t item_size;
  // Allocator *a;
} ArrayHeader;

// Array functions

void* __array_init(size_t item_size, size_t capacity /* Allocator* a */);
void __array_append(void** inArray, void* value);
int array_length(void* inArray);
