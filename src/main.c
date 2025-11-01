#include <array.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  Allocator a = {
      .alloc = malloc,
      .realloc = realloc,
      .free = free};

  int* dynArray = array(int, &a);

  for (int i = 0; i < 100; ++i) {
    array_append(&dynArray, &i);
  };

  for (int i = 0; i < 69; ++i) {
    array_pop(&dynArray);
  };

  int value = 3;

  array_add(&dynArray, 12, &value);
  for (int i = array_length(dynArray) - 1; i >= 0; --i) {
    printf("[%d]", dynArray[i]);
  };
  printf("\n");
  array_remove(&dynArray, 12);
  
  int* dynArrayb = array(int, &a);
  
  array_append(&dynArrayb, &value);
  
  array_merge(&dynArray, dynArrayb);
  
  for (int i = array_length(dynArray) - 1; i >= 0; --i) {
    printf("[%d]", dynArray[i]);
  };
  printf("\n");

  array_delete(dynArray);
  array_delete(dynArrayb);
}