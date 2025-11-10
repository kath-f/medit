#include "ME_File.hpp"

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>

//i want the constructor to open the file and find out what format it is,
//for this end im just looking at the raw bytes to look for the header.

ME::File::File(std::string path_to_file){
	std::fstream file(path_to_file.c_str(), std::ios_base::in);
	
	//im setting the type to null so i can check later wether it has already been set or not
	this->type = invalid;
	
	if(file.is_open()){
		this->name = path_to_file;
		
		//NOTE: this is probably very stupid way to do this
		//for wav:
		if(this->type == invalid){
			char word[4];
			file.seekg(8); //starting from byte 8
			file.read(word, 4); //read four bytes
			
			if(strcmp(word, "WAVE")==0){
				this->type = wav;
				
			}
		}

		//for mp3
		if(this->type == invalid){
			char word[3];
			file.read(word, 3);

			if(strcmp(word, "ID3")==0){
				this->type = mp3;
			}
		}

		//for flac

		if(this->type == invalid){
			char word[4];
			file.read(word, 4);

			if(strcmp(word, "fLaC")==0){
				this->type = flac;
			}
		}
	}
	else{
		std::cout << "Could not open " << path_to_file << std::endl; 
		exit(-1);
	}
}

std::string ME::File::getName(){
	return this->name;
}

ME::FileType ME::File::getType(){
	return this->type;
}

std::string ME::File::getTypeName(){
	if(this->type == wav){
		return "wav";
	}
	if(this->type == mp3){
		return "mp3";
	}
	if(this->type == flac){
		return "flac";
	}
	return "invalid";
}
