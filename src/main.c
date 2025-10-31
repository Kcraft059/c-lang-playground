#include <definitions.h>
#include <stdio.h>

int main() {
  char* string = array(char);
  char chr = 'H';
  array_append(&string,&chr);
  chr = 'i';
  array_append(&string,&chr);
  chr = '\0';
  array_append(&string,&chr);

  printf("%s, %d\n", string, array_length(string));
}