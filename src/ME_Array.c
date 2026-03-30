#include "ME_Array.h"

#include "ME_Print.h"

#include <stdlib.h>

void me_array_push(ME_Array* arr, void* obj){
  
  //increase size
  arr->size ++;
  //if size is >= capacity, double the capacity
  if(arr->size >= arr->capacity) arr->capacity *= 2;

  //reallocate the data to new capacity
  arr->data = realloc(arr->data, arr->capacity * sizeof(*arr->data));
  
  //pushes in the new entry
  arr->data[arr->size-1] = obj;
}

void* me_array_get(ME_Array* arr, size_t index){
  if(index > arr->size-1){
    me_print(ME_LOGLEVEL_WARN, "me_array_get(): tried to get entry at index higher than size");
    return NULL;
  }
  //else
  return arr->data[index];
}


void me_array_set(ME_Array* arr, size_t index, void* obj){
  if(index > arr->size-1){
    me_print(ME_LOGLEVEL_WARN, "me_array_set(): tried to set entry at index higher than size");
    return;
  }
  //else
  arr->data[index] = obj;
}

void me_array_remove(ME_Array* arr, size_t index){

  if(arr->size == 0){
    me_print(ME_LOGLEVEL_WARN,
	     "me_array_remove(): array is empty");
    return;
  }
  if(index > arr->size-1){
    me_print(ME_LOGLEVEL_WARN,
	     "me_array_remove(): tried to remove entry at higher index than size");
    return;
  }

  //pastes it into the original array, overriding the entry at index
  for(size_t i=index ; i<arr->size - 1; i++){
    arr->data[i] = arr->data[i+1];
  }

  //truncates the array
  arr->size --;
  arr->data = realloc(arr->data, arr->size * sizeof(*arr->data));
}

//frees all the entries AND the array object istself
void me_array_free(ME_Array* arr){
  arr->size = 0;
  free(arr->data);
}
