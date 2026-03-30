#include "ME_File.h"

#include "ME_Print.h"
#include "ME_List.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int me_openfile(ME_File* file){

  //this function uses the file's name to open it
  if(file->name == NULL){
    me_print(ME_LOGLEVEL_ERROR, "File NAME was not initialized");
    return -1;
  }

  file->cfile = fopen(file->name, "r+");
  if(file->cfile == NULL){
    me_print(ME_LOGLEVEL_ERROR, "me_openfile(): could not open file (%s)", file->name);
    return -1;
  }

  me_print(ME_LOGLEVEL_DEBUG, "File opened successfully: %s", file->name);

  //looks for the metadata
  me_try_list(file);

  //gets the fields for the metadata
  for(size_t i=0; i<file->metadata_arr.size; i++){
    ME_Metadata* meta = (ME_Metadata*)me_array_get(&(file->metadata_arr), i);
    
    switch(meta->fmt){
    case ME_FORMAT_LIST_INFO:
      me_get_list_info_fields(file);
      break;
    case ME_FORMAT_ID3v2:
      break;
    case ME_FORMAT_UNDEFINED:
      break;
    }
  }

  //prints the info found
  for(size_t i=0; i<file->metadata_arr.size; i++){
    ME_Metadata* meta = (ME_Metadata*)me_array_get(&(file->metadata_arr), i);
    
    me_print(ME_LOGLEVEL_DEBUG, "Format: %s, offset: %u",
	     ME_FMT_STR_TABLE[meta->fmt], meta->offset);
    
    for(size_t j=0; j<meta->field_arr.size; j++){
      ME_MetadataField* field = (ME_MetadataField*)me_array_get(&(meta->field_arr), j);
      
      me_print(ME_LOGLEVEL_DEBUG, "ID: %s, offset: %u, value: %s",
	       field->id, field->offset, field->value);
    }
  }

  return 0;
}

void me_closefile(ME_File* file){
  
  //fclose(NULL) is undefined behaviour
  if(file->cfile != NULL){
    me_print(ME_LOGLEVEL_DEBUG, "Closing file %s", file->name);
    fclose(file->cfile);
  }

  //frees each entry in the metadata_arr
  for(size_t i=0; i<file->metadata_arr.size; i++){
    ME_Metadata* meta = (ME_Metadata*)me_array_get(&(file->metadata_arr), i);

    //free each entry in the metadata's field_arr
    for(size_t j=0; j< meta->field_arr.size; j++){
      ME_MetadataField* field = (ME_MetadataField*)me_array_get(&(meta->field_arr), j);
      
      //frees the field_arr entry's id and value
      free(field->id);
      free(field->value);

      free(field);
    }
    me_array_free(&(meta->field_arr));
    free(meta);
  }
  me_array_free(&(file->metadata_arr));

  free(file->name);
}

void me_fread_bytes(FILE* cfile, size_t n, void* buf){
  fread(buf, 1, n, cfile);
}

uint32_t me_fread_u32le(FILE* cfile){
  uint8_t byte_arr[4];
  me_fread_bytes(cfile, 4, byte_arr);

  return (((uint32_t) byte_arr[3]) << 24 |
	  ((uint32_t) byte_arr[2]) << 16 |
	  ((uint32_t) byte_arr[1]) << 8  |
	  ((uint32_t) byte_arr[0]));
}

uint32_t me_fread_u32be(FILE* cfile){
  uint8_t byte_arr[4];
  me_fread_bytes(cfile, 4, byte_arr);

  return (((uint32_t) byte_arr[0]) << 24 |
	  ((uint32_t) byte_arr[1]) << 16 |
	  ((uint32_t) byte_arr[2]) << 8  |
	  ((uint32_t) byte_arr[3]));
}

uint32_t me_fread_u32le_synchsafe(FILE* cfile){
  uint8_t byte_arr[4];
  me_fread_bytes(cfile, 4, byte_arr);

  return (((uint32_t) byte_arr[3] & 0b0111111) << 21 |
	  ((uint32_t) byte_arr[2] & 0b0111111) << 14 |
	  ((uint32_t) byte_arr[1] & 0b0111111) << 7  |
	  ((uint32_t) byte_arr[0] & 0b0111111));
}

uint32_t me_fread_u32be_synchsafe(FILE* cfile){
  uint8_t byte_arr[4];
  me_fread_bytes(cfile, 4, byte_arr);

  return (((uint32_t) byte_arr[0] & 0b0111111) << 21 |
	  ((uint32_t) byte_arr[1] & 0b0111111) << 14 |
	  ((uint32_t) byte_arr[2] & 0b0111111) << 7  |
	  ((uint32_t) byte_arr[3] & 0b0111111));
}

void me_fwrite_bytes(FILE* cfile, size_t n, void* buf){
  fwrite(buf, 1, n, cfile);
}

void me_fwrite_u32le(FILE* cfile, uint32_t obj){
  uint8_t bytes[4];
  bytes[3] = (uint8_t) (obj >> 24);
  bytes[2] = (uint8_t) (obj >> 16);
  bytes[1] = (uint8_t) (obj >> 8 );
  bytes[0] = (uint8_t) (obj);

  me_fwrite_bytes(cfile, 4, bytes);
}

void me_fwrite_u32be(FILE* cfile, uint32_t obj){
  uint8_t bytes[4];
  bytes[0] = (uint8_t) (obj >> 24);
  bytes[1] = (uint8_t) (obj >> 16);
  bytes[2] = (uint8_t) (obj >> 8 );
  bytes[3] = (uint8_t) (obj);

  me_fwrite_bytes(cfile, 4, bytes);
}

void me_fwrite_u32le_synchsafe(FILE* cfile, uint32_t obj){
  uint8_t bytes[4];
  bytes[3] = (uint8_t) ((obj & 0x7f000000) >> 21);
  bytes[2] = (uint8_t) ((obj & 0x007f0000) >> 14);
  bytes[1] = (uint8_t) ((obj & 0x00007f00) >> 7 );
  bytes[0] = (uint8_t) ((obj & 0x0000007f));

  me_fwrite_bytes(cfile, 4, bytes);
}

void me_fwrite_u32be_synchsafe(FILE* cfile, uint32_t obj){
  uint8_t bytes[4];
  bytes[0] = (uint8_t) ((obj & 0x7f000000) >> 21);
  bytes[1] = (uint8_t) ((obj & 0x007f0000) >> 14);
  bytes[2] = (uint8_t) ((obj & 0x00007f00) >> 7 );
  bytes[3] = (uint8_t) ((obj & 0x0000007f));

  me_fwrite_bytes(cfile, 4, bytes);
}
