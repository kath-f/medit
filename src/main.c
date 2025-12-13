#include <stdio.h>
#include "ME_File.h"

int main(int argc, char** argv){
	
	if(argc == 2){
		ME_File* file = ME_fileOpen(argv[1]);
		if(file->cfile==nullptr){
			printf("Could not open file\n");
			ME_fileClose(file);
			return -1;	
		}
		if(file->ftype == NOT_SUPPORTED){
			printf("File type not supported \n");
			ME_fileClose(file);
			return -1;
		}
		
		printf("File %s opened successfully, file type %i \n", file->fname, file->ftype);
		ME_fileClose(file);
		
		return 0;
	}
	printf("Not enough or too many arguments\n");
	return -1;
}
