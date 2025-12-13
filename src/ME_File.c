#include "ME_File.h"

#include <stdlib.h>
#include <string.h>


static ME_FileType getType(FILE* cfile){
	rewind(cfile);

	unsigned char id[4];
	fread(id, 1, 4, cfile);
	
	if(strncmp((const char*) id, "RIFF", 4) == 0){
		fseek(cfile, 8, 0);
		fread(id, 1, 4, cfile);
		
		if(strncmp((const char*) id, "WAVE", 4) == 0){
			return WAVE;
		}
	}
	else if(strncmp((const char*) id, "fLaC", 4) == 0){
		return FLAC;
	}
	else if((id[0] == 'I' && id[1] == 'D' && id[2] == '3') ||
		(id[0] == 0xff && id[1] == 0xff && id[2] >= 0xE0)){
		return MP3;
	}

	return NOT_SUPPORTED;
}

ME_File* ME_fileOpen(const char* file_name){
	ME_File* file = malloc(sizeof(ME_File));
	file->fname = file_name;

	file->cfile = fopen(file->fname, "r+");
	if(file->cfile == nullptr) return file;

	file->ftype = getType(file->cfile);
	return file;
}

void ME_fileClose(ME_File* file){
	if(file->cfile != nullptr)fclose(file->cfile);
	free(file);
}
