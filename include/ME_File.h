#ifndef ME_FILE_H
#define ME_FILE_H

#include <stdio.h>

typedef enum ME_FileType{
	MP3,
	WAVE,
	FLAC,
	NOT_SUPPORTED
} ME_FileType;

typedef struct ME_File{
	FILE* cfile;
	const char* fname;
	ME_FileType ftype;
} ME_File;

ME_File* ME_fileOpen(const char* file_name);
void ME_fileClose(ME_File* file);
#endif
