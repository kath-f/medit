#ifndef __ME_METADATA_H
#define __ME_METADATA_H

#include "ME_Array.h"

#include <stddef.h>

typedef enum{
  ME_FORMAT_UNDEFINED,
  ME_FORMAT_ID3v2,
  ME_FORMAT_LIST_INFO,
} ME_MetadataFormat;

extern const char* ME_FMT_STR_TABLE[];

typedef struct{
  char* id;
  char* value;

  size_t offset; //the offset points to after the ID
} ME_MetadataField;

typedef struct{
  ME_MetadataFormat fmt;
  
  size_t offset; //this should point to the location in the file immediately after id
  size_t size; //size is as is in the file
  
  ME_Array field_arr;
  
} ME_Metadata;

#endif
