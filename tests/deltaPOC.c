#include <stdlib.h>

/**
 * I'll hate myself for not having explained those…
 */

struct diffList {
  int length;
  void* array;
};

void getDelta(void* list_a, void* list_b, struct diffList* list, size_t size, size_t itemSize) {
  uint8_t* a = list_a;
  uint8_t* b = list_b;
  list->array = malloc(size);
  list->length = 0;
  int offset = -1;
  for (int i = 0; i < size; ++i) {
    uint8_t diff = a[i] ^ b[i];
    if (diff != 0 && offset != i / itemSize) {
      offset = i / itemSize;
      printf("We're at index : %d, pos : %d, a : %d, b: %d\n", i, offset, ((int*)a)[offset], ((int*)b)[offset]);
      for (int j = 0; j < itemSize; ++j) {
        int index = offset * itemSize + j;
        printf("We're at offset : %d\n", index);
        ((uint8_t*)(list->array))[list->length * itemSize + j] = a[index] ^ b[index];
      }
      list->length++;
    }
  }
}

int main(int argc, char** argv) {
  int list_og[] = {3, 640, 4};
  int list_new[] = {2, 3879893, 4};

  struct diffList* list = malloc(sizeof(struct diffList));
  getDelta(list_og, list_new, list, 3 * sizeof(int), sizeof(int));

  int* delta = ((int*)(list->array));

  for (int i = 0; i < list->length; ++i) {
    int newVal = delta[i] ^ list_og[i];
    printf("newVal %d\n", newVal);
  }

  free(list->array);
  free(list);
}
