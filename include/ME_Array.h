#ifndef __ME_ARRAY_H
#define __ME_ARRAY_H

#include <stddef.h>

typedef struct{
  void** data; //array of void*
  
  size_t size;
  size_t capacity;
  
}ME_Array;
#define ME_ARRAY_INIT ((ME_Array) {.data = NULL, .size=0, .capacity=8})

void me_array_push(ME_Array* arr, void* obj);
void* me_array_get(ME_Array* arr, size_t index);
void me_array_set(ME_Array* arr, size_t index, void* obj);
void me_array_remove(ME_Array* arr, size_t index);
void me_array_free(ME_Array* arr);

#endif
