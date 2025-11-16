## C lang Playground repository

Here are some little tests I made while trying to learn C.
They do not have any real purpose, I might do a project in C/C++ at one point.

## What might be a bit interesting in what i did 

(dynamic array & hashmap implementation)

https://github.com/Kcraft059/c-lang-playground/blob/5f3ef4d7d2930d41c120871fcc9fde015db50bb6/src/include/array.h
https://github.com/Kcraft059/c-lang-playground/blob/5f3ef4d7d2930d41c120871fcc9fde015db50bb6/src/array.c

> [!NOTE]
> If you have any ressources / ideas that could help me learn c, feel free to share them here ^^ !

## Performance note

```c
#include <stdio.h>
int main() {
  long unsigned i;
  for (i = 0; i < 10000000; i++) {
    long x = i * 2; // A simple operation
  }
  printf("Done : %lu\n",i);
}
```

```bash
time ./out/exec
# Done : 10000000
# ./out/exec  0.01s user 0.00s system 94% cpu 0.014 total
```

```python
i = 0
while i < 10000000 :
  i = i + 1
  x = i * 2 # A simple operation
print("Done : ",i)
```
```bash
time python3 test.py
# Done :  10000000
# python3 test.py  0.73s user 0.01s system 99% cpu 0.736 total
```
