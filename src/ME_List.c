#define _GNU_SOURCE
#include <unistd.h>

#include "ME_List.h"

#include "ME_Print.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


// START OF me_try_list() ---------------------------------------------------------------------
void me_try_list(ME_File* file){

//FOOLPROOFING --------------------------------------------------------------------------------
  rewind(file->cfile);
  
  //check for id3v2 tags at the start of the file, then skips them
  for(size_t i=0; i<file->metadata_arr.size; i++){
    ME_Metadata* meta = (ME_Metadata*)me_array_get(&(file->metadata_arr), i);
    if(meta->fmt == ME_FORMAT_ID3v2 && meta->offset == strlen("ID3")){
      fseek(file->cfile, meta->offset + 3, SEEK_SET);
      size_t chunk_size = me_fread_u32le_synchsafe(file->cfile);
      fseek(file->cfile, chunk_size, SEEK_CUR);
    }
  }
  
  char id[5];
  id[4] = '\0';
  me_fread_bytes(file->cfile, 4, id);
  
  if(strcmp(id, "RIFF")!=0) return; //if the first 4 bytes aren't riff, give up
//END OF FOOLPROOFING -------------------------------------------------------------------------
  fseek(file->cfile, 8, SEEK_CUR); //skip the 4 bytes of RIFF chunk size + 4 bytes of
                                   //format identifier
//READING CHUNKS ------------------------------------------------------------------------------
  //will keep reading new chunks until it hits the data chunk
  while(feof(file->cfile) == 0){
    me_fread_bytes(file->cfile, 4, id);

    //READING LIST ----------------------------------------------------------------------------
    if(strcmp(id, "LIST")==0){

      size_t offset = ftell(file->cfile);
     
      size_t chunk_size = me_fread_u32le(file->cfile); //reads over the chunk bytes
      
      me_fread_bytes(file->cfile, 4, id); //looks for the LIST format ID
      if(strcmp(id, "INFO")==0){
	//makes the new entry then pushes it into the array
	ME_Metadata* new_entry;
	new_entry = malloc(sizeof(ME_Metadata));
	
	new_entry->fmt = ME_FORMAT_LIST_INFO;
	new_entry->offset = offset;

	new_entry->field_arr = ME_ARRAY_INIT;
	
	me_array_push(&(file->metadata_arr), new_entry);
	
      }else{
	me_print(ME_LOGLEVEL_WARN, "LIST format not recognized");
      }

      // skips the rest of the chunk
      fseek(file->cfile, (offset + strlen("LIST")) + chunk_size, SEEK_SET);
    }
    //END OF READING LIST ---------------------------------------------------------------------
    
    //READING ID3 -----------------------------------------------------------------------------
    else if(strncmp(id, "ID3", 3)==0){
      
      //subtract 1 becasue the ID3 id is 3 bytes long and we read 4
      size_t offset = ftell(file->cfile) - 1;
      //the byte we are currently in would be major version
      
      //skips over the revision number + flags byte
      fseek(file->cfile, 2, SEEK_CUR);

      //read the size
      size_t chunk_size = me_fread_u32le_synchsafe(file->cfile);
	
      //create entry, then push into metadata array
      ME_Metadata* new_entry;
      new_entry = malloc(sizeof(ME_Metadata));
	
      new_entry->fmt = ME_FORMAT_ID3v2;
      new_entry->offset = offset;
      
      new_entry->field_arr = ME_ARRAY_INIT;
      
      me_array_push(&(file->metadata_arr),  new_entry);

      //skips the rest of the chunk
      fseek(file->cfile, (offset + strlen("ID3")) + chunk_size, SEEK_SET);
    }
    //END OF READING ID3-----------------------------------------------------------------------
    
    //if it reached the data chunk stop reading
    else if(strcmp(id, "data")==0){
      break;
    }
    //if it doesn't find a metadata chunk...
    else{
      //reads the chunk size for the current chunk then skips over by that much
      uint32_t chunk_size = me_fread_u32le(file->cfile);
      fseek(file->cfile, chunk_size, SEEK_CUR);
    }
  }
//END OF READING CHUNKS -----------------------------------------------------------------------
}
//END of me_try_list() ------------------------------------------------------------------------


// START OF me_get_list_info_fields() ---------------------------------------------------------
void me_get_list_info_fields(ME_File* file){

// FOOLPROOFING -------------------------------------------------------------------------------
  //check if there is an info chunk
  int info_index = -1;
  for(size_t i=0; i<file->metadata_arr.size; i++){
    if(((ME_Metadata*)file->metadata_arr.data[i])->fmt == ME_FORMAT_LIST_INFO){
      info_index = i; //if there is, store the index
    }
  }
  //and if there is none just return
  if(info_index == -1){
    me_print(ME_LOGLEVEL_WARN, "me_get_list_info_field(): file %s has no LIST_INFO chunks",
	     file->name);
    return;
  }

  ME_Metadata* info_meta = (ME_Metadata*)me_array_get(&(file->metadata_arr), info_index);
// END OF FOOLPROOFING ------------------------------------------------------------------------

// READING THE FILE ---------------------------------------------------------------------------
  //skip to the offset of the chunk
  fseek(file->cfile, info_meta->offset, SEEK_SET);
  size_t info_meta_size = me_fread_u32le(file->cfile);

  fseek(file->cfile, 4, SEEK_CUR); //skips the format ID

  while((size_t)ftell(file->cfile) < info_meta_size + info_meta->offset){      
    char id[5];
    id[4]='\0';
    me_fread_bytes(file->cfile, 4, id); //store the id

    size_t offset = ftell(file->cfile);

    size_t value_size = me_fread_u32le(file->cfile); //read size of the value
    char value[value_size];
    me_fread_bytes(file->cfile, value_size, value); //store the value

    //skip padding byte if found
    if(value_size % 2 != 0) fseek(file->cfile, 1, SEEK_CUR);
// END OF READING THE FILE --------------------------------------------------------------------

// CREATING FIELD ENTRY -----------------------------------------------------------------------
    ME_MetadataField* new_entry;
    new_entry = malloc(sizeof(ME_MetadataField));

    new_entry->id = malloc(sizeof(char) * 5);
    strcpy(new_entry->id, id);

    new_entry->value = malloc(sizeof(char) * value_size);
    strcpy(new_entry->value, value);

    new_entry->offset = offset; //the offset is the point after the ID

    me_array_push(&(info_meta->field_arr), new_entry);
// END OF CREATING FIELD ENTRY ----------------------------------------------------------------
  }
}
//END OF me_get_list_info_fields() ------------------------------------------------------------


// START OF me_remove_list_info_field() -------------------------------------------------------
int me_remove_list_info_field(ME_File* file, size_t field_index){
//FOOLPROOFING --------------------------------------------------------------------------------
  ME_Metadata* info_meta = NULL;
  ME_MetadataField* field = NULL;
  size_t riff_offset = 4;
  
  for(size_t i=0; i<file->metadata_arr.size; i++){
    ME_Metadata* meta = (ME_Metadata*)me_array_get(&(file->metadata_arr), i);
    switch(meta->fmt){
    case ME_FORMAT_ID3v2: //check if there is an id3v2 chunk at the start of file
      if(meta->offset == strlen("ID3")){
	//skips over 2 bytes of version + 1 byte of flags
	fseek(file->cfile, meta->offset + 3, SEEK_SET);
	uint32_t chunk_size = me_fread_u32le(file->cfile);
	fseek(file->cfile, chunk_size, SEEK_CUR);

	char id[5];
	id[4] = '\0';
	me_fread_bytes(file->cfile, 4, id);
	if(strcmp(id, "RIFF")==0){
	  riff_offset = ftell(file->cfile);
	}else{
	  me_print(ME_LOGLEVEL_WARN, "%s%s" //i'm breaking this up just so it looks better
		   "me_remove_list_info_field(): ID3 header found at start of file, ",
		   "but could not figure out RIFF chunk offset");
	  return -1;
	}	 
      }
      break;
    case ME_FORMAT_LIST_INFO:
      if(info_meta != NULL){
	me_print(ME_LOGLEVEL_WARN, "me_remove_list_info_field(): multiple INFO entries");
	return -1;
      }else{
	field = (ME_MetadataField*)me_array_get(&(meta->field_arr), field_index);
	info_meta = meta;
      }
      break;
    case ME_FORMAT_UNDEFINED:
      break;
    }
  }

  if(info_meta == NULL){
    me_print(ME_LOGLEVEL_WARN,
	     "me_remove_list_info_field(): INFO entry was not found in file->metadata_arr");
    return -1;
  }
  if(field == NULL){
    me_print(ME_LOGLEVEL_WARN,
	     "me_remove_list_info_field(): field %u not found in info_meta->field_arr",
	     field_index);
    return -1;
  }
  
// BUFFERING FILE -----------------------------------------------------------------------------
  //get offset of the end of the file
  fseek(file->cfile, 0, SEEK_END);
  size_t file_end = ftell(file->cfile);

  fseek(file->cfile, field->offset, SEEK_SET); //jump to start of field
  size_t value_size = me_fread_u32le(file->cfile); //read the size of value
  
  //skips over the value, accounting for padding bytes
  fseek(file->cfile, value_size + ((value_size % 2==0) ? 0 : 1), SEEK_CUR);

  size_t original_offset = ftell(file->cfile);

  //read from the point this field ends to the end of the file into buffer
  uint8_t rest_of_file[file_end - original_offset]; 
  me_fread_bytes(file->cfile, file_end - original_offset, rest_of_file);
  
// OVERWRITING --------------------------------------------------------------------------------
  
  //go back to the original position of the field (start of ID)
  fseek(file->cfile, field->offset - 4, SEEK_SET);
  
  //ovewrite the field
  me_fwrite_bytes(file->cfile, file_end - original_offset, rest_of_file);

  //finally truncate the file to the new size
  ftruncate(fileno(file->cfile), (file_end - (value_size + ((value_size%2==0) ? 0 : 1)) - 8));
  
// UPDATING SIZE ------------------------------------------------------------------------------

  fseek(file->cfile, riff_offset, SEEK_SET);
  size_t chunk_size = me_fread_u32le(file->cfile);
  fseek(file->cfile, -4, SEEK_CUR);
  me_fwrite_u32le(file->cfile, chunk_size - 8 - (value_size + ((value_size%2==0)?0:1)));

  fseek(file->cfile, info_meta->offset, SEEK_SET);
  chunk_size = me_fread_u32le(file->cfile);
  fseek(file->cfile, -4, SEEK_CUR);
  me_fwrite_u32le(file->cfile, chunk_size - 8 - (value_size + ((value_size%2==0)?0:1)));

// RECOMPUTING OFFSET -------------------------------------------------------------------------
  for(size_t i=0; i<file->metadata_arr.size; i++){
    ME_Metadata* meta = (ME_Metadata*)me_array_get(&(file->metadata_arr), i);
    if(meta->offset > info_meta->offset){
      //subtracts the number of bytes removed from the offset
      meta->offset -= 8 - (value_size + ((value_size%2==0)?0:1));
    }
  }

// REMOVING FIELD_ARR ENTRY -------------------------------------------------------------------

  free(field->id);
  free(field->value);
  free(field);
  me_array_remove(&(info_meta->field_arr), field_index);
  
  return 0;
}
// END OF me_remove_list_info_field() ---------------------------------------------------------

size_t me_get_riff_offset(ME_File* file){
  size_t offset = 4;
  for(size_t i=0; i<file->metadata_arr.size; i++){
    ME_Metadata* meta = (ME_Metadata*)me_array_get(&(file->metadata_arr), i);
    if(meta->fmt == ME_FORMAT_ID3v2 && meta->offset == 3){
      //offset of id3 chunk + the size of id3 chunk
      //+ 2 bytes version + 1 byte flag + 4 bytes RIFF id
      offset = meta->offset + meta->size + 3 + 4;
    }
  }
  //checking if the offset does point to RIFF id
  fseek(file->cfile, -4, SEEK_CUR);
  char id[4];
  me_fread_bytes(file->cfile, 4, id);
  if(strncmp(id, "RIFF", 4)==0){
    return offset;
  }
  else{
    return -1;
  }
}
