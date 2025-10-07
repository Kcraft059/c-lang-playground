#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* int * */
void expand_array(int **original_array, int length, int new_length) // Is basically a less shiny realloc()
{
  unsigned long byte_length = sizeof(**original_array) * length;
  unsigned long new_byte_length = sizeof(**original_array) * new_length;
  int *returned_array = malloc(new_byte_length);

  if (length < new_length)
  {
    memcpy(returned_array, *original_array, byte_length);
  } 
  else
  {
    memcpy(returned_array, *original_array, new_byte_length);
  }

  // *(returned_array + sizeof(original_array) + 0 * sizeof(int)) = added_value ;
  free(*original_array);
  *original_array = returned_array;
  // return returned_array;
}

int main()
{
  // int **pointer_array = malloc(4 * sizeof(int *)); // Doesn't need to be a pointer itself
  int *pointer_array[4];
  // int *array = malloc(4 * sizeof(int));
  // pointer_array[1] = array;
  // array = NULL; // Can unset value of array since pointer is stored
  pointer_array[0] = malloc(4 * sizeof(int));

  int tmp[] = {3, 4, 5, 8};
  memcpy(pointer_array[0], tmp, sizeof(tmp));

  /* pointer_array[0] =  */
  //expand_array(&pointer_array[0], 4, 3);
  pointer_array[0] = realloc(pointer_array[0], sizeof(*pointer_array[0]) * 2);
  // pointer_array[0][5] = 3;

  // printf("%d\n", *(*(pointer_array + 0) + 2));
  printf("%d, %d\n", pointer_array[0][1], pointer_array[0][1]);

  free(pointer_array[0]);
  // free(pointer_array);

  return 0;
}

/**
 * In c a type is tied to a unit size
 *
 * [for example] :
 * sizeof(long) == 8
 *
 * from that on, it reads the number of bytes from the given adress depending on the type size
 *
 * long *x = malloc(sizeof(long));
 * x[0] -> reading from pointer adress -> pointer adress + size of type 
 *
 *
 */