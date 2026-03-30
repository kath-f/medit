#ifndef __ME_FILE_H
#define __ME_FILE_H

#include "ME_Metadata.h"
#include "ME_Array.h"

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

typedef struct{
  ME_Array metadata_arr; //array of metadata chunks

  FILE* cfile; 
  char* name;
  
} ME_File;
#define ME_FILE_NULL ((ME_File) {.metadata_arr=ME_ARRAY_INIT,	\
				 .cfile=NULL,\
				 .name=NULL})
/*
  PLEASE I BEG OF YOU, make sure you define a ME_File as ME_FILE_NULL before doing anything
  with it.
 
  there once was a little gurl,
  who didn't initialize her struct to NULL
  she passed it around
  and got a segfault
  and didn't konw where it came from

  (kinda rhymes but not really)
 */

int me_openfile(ME_File* file); //function for opening files
void me_closefile(ME_File* file); //function for closing files

//lower level file IO functions, basically utility
void me_fread_bytes(FILE* cfile, size_t n, void* buf);
uint32_t me_fread_u32le(FILE* cfile);
uint32_t me_fread_u32be(FILE* cfile);
uint32_t me_fread_u32le_synchsafe(FILE* cfile);
uint32_t me_fread_u32be_synchsafe(FILE* cfile);

void me_fwrite_bytes(FILE* cfile, size_t n, void* buf);
void me_fwrite_u32le(FILE* cfile, uint32_t obj);
void me_fwrite_u32be(FILE* cfile, uint32_t obj);
void me_fwrite_u32le_synchsafe(FILE* cfile, uint32_t obj);
void me_fwrite_u32be_synchsafe(FILE* cfile, uint32_t obj);
#endif
