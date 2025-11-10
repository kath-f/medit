#include "ME_Parser.hpp"
#include "ME_File.hpp"

#include <iostream>
#include <cstdlib>

//this really should be something that's called once at the very start of the file, and have only one.
//the format is medit input_file.mp3 get/set track/artist/album/etc "some value" in that order

//TODO: check so the input file is a wav file and store it's format

//WRITTEN WHILE VERY SLEEPY AND ANXIOUS ABOUT THE BIG TEST TOMORROW, but has to be done

ME::Parser::Parser(int argc, char** argv){

	for(int i=1; i<argc; i++){ //i'm starting from one, since the first argument is always the program name
		//i'm converting it to std::str bcs i dont feel like using C strcmp
		std::string arg = argv[i];

		if(arg == "set" or arg == "get"){
			//sets the first comamnd to the current argv of the loop (set or get)
			//and sets the attribute to the argv after that (which should be title, artist, etc).
			this->command = argv[i];
			this->attribute = argv[i+1];
			
			//start a new search from this point to the end for values if the arg is "set"
			//if the value is "get" there are no values to be assigned
			if(arg == "set"){
				for(int j=i+2; j<argc; j++){
					this->value.push_back(argv[j]);
				}
			}
			//after getting all the values, return from the function
			break;
		}
		else{
			//this whole making a temporaryu file and checking if its invalid is important
			//so you cant input an image file or smt
			ME::File tempFile(arg);
			if(tempFile.getType() != invalid) this->input_file.push_back(tempFile);
		}

		//itll take arg as an input_file until it hits a set or a get, at which point it will
		//get thrown into a second loop, which will retunr from the function at the end.
	}
	
	//if there is more than 1 value then there should be a matching number of values and inputs
	if((value.size() > 1) && (input_file.size() != value.size())){
		std::cout << "Not enough values for number of inputs!" << std::endl;
		exit(-1);
	}

	//checks if no commands or attributes were given
	if((command.empty()) || (attribute.empty())){
		std::cout << "No attributes or commands given" << std::endl;
		exit(-1);
	}

	//checks if no valid inputs were given
	if(this->input_file.empty()){
		std::cout << "No valid inputs given" << std::endl;
		exit (-1);
	}
}


//just some simple getter funcitons
std::vector <ME::File> ME::Parser::getInputFile(){
	return this->input_file;
}

std::string ME::Parser::getCommand(){
	return this->command;
}

std::string ME::Parser::getAttribute(){
	return this->attribute;
}

std::vector <std::string> ME::Parser::getValue(){
	return this->value;
}
