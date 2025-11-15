#include "ME_Core.hpp"

#include <iostream>
#include <string>

#include <taglib/tbytevector.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/textidentificationframe.h>

//this is the function i want the main file to use when it wants to "run" the program
int ME::Core::main(int argc, char** argv){
	this->errorFlag = false;

	this->parse(argc, argv);
	this->checkForErrors();
	if(errorFlag == false){
		if(this->command == "get"){ 
			this->runGet();
		}
	
		else if(this->command == "set"){
			this->runSet();
		}

		return 0;
	}
	return -1;
}

void ME::Core::parse(int argc, char** argv){

	for(int i=1; i<argc; i++){ //starting from 1 since 0 is always program name
		
		//if it find that the current argv is a command(set or get)
		if(std::string(argv[i])=="set" or std::string(argv[i])=="get"){
			//it will mark the command as the current argv
			this->command = argv[i];
			//and mark the attribute as the next argv over
			this->attribute = argv[i+1];

			//if it is set i will start a new search from 2 argvs over (1 after attribute)
			//to look for values
			//get doesnt need values
			if(std::string(argv[i])=="set"){
				for(int j=i+2; j<argc; j++){
					this->values.push_back(argv[j]);
				}
			}
			
			//after this it should be all done
			break;
		}
		//if it doesnt find a command, it will take the argv to be an input file
		else{
			//i'm making a temporary instance of the file so i can check wether it's valid
			//before including it
			TagLib::FileRef temp_file(argv[i]);
			if(!(temp_file.isNull())){
				this->files.push_back(temp_file);
			}
		}
	}

	//if there are multiple files, but only one value, then i to copy the first value until the number of files and values match
	if(files.size() > 1 and values.size()==1){
		for(int i=1; i<files.size(); i++){
			values.push_back(values.at(0));
		}
	}
}

void ME::Core::checkForErrors(){
	//these are error conditions in which the program will quit:
	
	//if there were no files inputted
	if(this->files.empty()){
		std::cout << "ERROR: No valid input files given" << std::endl;
		this->errorFlag = true;
	}

	//if no command was given
	if(this->command.isEmpty()){
		std::cout << "ERROR: No commands given" << std::endl;
		this->errorFlag = true;
	}

	//if no attribute was given
	if(this->attribute.isEmpty()){
		std::cout << "ERROR: No attribute given" << std::endl;
		this->errorFlag = true;
	}
	//if an invalid attribute was given
	else if(this->attribute != "title" and
		this->attribute != "artist" and
		this->attribute != "album" and
		this->attribute != "comment" and
		this->attribute != "genre" and
		this->attribute != "year" and
		this->attribute != "track"){
		
		std::cout << "ERROR: Invalid attribute" << std::endl;
		this->errorFlag = true;
		
	}

	//if command set was used but no values were given
	if(this->command == "set" and this->values.empty()){
		std::cout << "ERROR: SET command used, but no values given" << std::endl;
		this->errorFlag = true;
	}

	//if more than one value was given but they don't match the number of files
	if(command == "set" and files.size() != values.size()){
		std::cout << "ERROR: More than one value given, but it doesn't match the number of files" << std::endl;
		this->errorFlag = true;
	}
}

void ME::Core::runGet(){

	for(int i=0; i<this->files.size(); i++){
		
		TagLib::String tag;
		if(this->attribute == "title"){ tag = files.at(i).tag()->title();}
		else if(this->attribute == "artist"){ tag = files.at(i).tag()->artist(); }
		else if(this->attribute == "album"){ tag = files.at(i).tag()->album(); }
		else if(this->attribute == "comment"){ tag = files.at(i).tag()->comment(); }
		else if(this->attribute == "genre"){ tag = files.at(i).tag()->genre(); }
		else if(this->attribute == "year"){ tag = std::to_string(files.at(i).tag()->year()); }
		else if(this->attribute == "track"){ tag = std::to_string(files.at(i).tag()->track()); }
		
		this->outputs.push_back(tag);
		std::cout << files.at(i).file()->name() << " " << this->attribute << ": "
			<< tag << std::endl;
	}
}

void ME::Core::runSet(){
	
	for(int i=0; i<files.size(); i++){
		if(!files.at(i).tag() or files.at(i).tag()->isEmpty()){
			std::cout << files.at(i).file()->name() <<": file tag missing" << std::endl;
			
			files.at(i).file()->seek(0); 
			TagLib::ByteVector fmt_tag = files.at(i).file()->readBlock(4);	

			//FOR WAV (have to find the riff tag first)
			if(fmt_tag == TagLib::ByteVector("RIFF", 4)){
				files.at(i).file()->seek(8);
				fmt_tag = files.at(i).file()->readBlock(4);	

				if(fmt_tag == TagLib::ByteVector("WAVE", 4)){
					std::cout << "\tFile identified as WAVE" << std::endl;
					//TODO: For this one i'll have to actually inject the binary data for tags
					std::cout << "\tWAVE file metadata creation not supported rn, soweeyyyy :3" << std::endl;
					continue;
				}
			}

			//FOR MP3
			//the mp3 format doesn have a format ID like other formats, instead it has 11 "sync" bits
			//this is kinda annoying.
			else if((fmt_tag.at(0) == (char) 0xff) && (fmt_tag.at(1) >= (char) 0xf0)){
				
				//after identifying the file i'll construct it as an MPEG::File
				std::cout << "\tFile identified as MP3" << std::endl;
				TagLib::MPEG::File mpeg_file(files.at(i).file()->name());
				
				std::cout << "\tCreating tag..." << std::endl;
				mpeg_file.ID3v2Tag(true); //this automatically creates a tag field if the file is missing
				if(mpeg_file.save()){ //then save the file
					std::cout << "\tFile saved with tag" << std::endl;
					
					//i create a FileRef based on the file
					TagLib::FileRef mpeg_fileRef(&mpeg_file);
					files.at(i) = mpeg_fileRef; //assign it to the place of the old file ref's on the vector

					if(files.at(i).tag()){ //check again if the file was created just to be sure
						std::cout << "\tTag created, populating..." << std::endl;
						
						//call populateTags with true, so it created "empty" fields
						populateTags(true, i);
						
						//save the file
						files.at(i).file()->save();
						continue; //go on to the next file
					}
					std::cout << "\tCould not create tag" << std::endl;
				}

				std::cout << "\tCould not save file with new tag" << std::endl;
			}
		}
		else{
			populateTags(false, i);
		}

		files.at(i).save();
	}
}

void ME::Core::populateTags(bool createTags, int i){
	if(createTags){
		//creating the tags empty at first so they are all created
		files.at(i).tag()->setTitle(" ");
		files.at(i).tag()->setArtist(" ");
		files.at(i).tag()->setAlbum(" ");
		files.at(i).tag()->setComment(" ");
		files.at(i).tag()->setGenre(" ");
		files.at(i).tag()->setYear(0);
		files.at(i).tag()->setTrack(0);
		
	}

	if(this->attribute == "title"){ files.at(i).tag()->setTitle(values.at(i)); }
	else if(this->attribute == "artist"){ files.at(i).tag()->setArtist(values.at(i)); }
	else if(this->attribute == "album"){ files.at(i).tag()->setAlbum(values.at(i)); }
	else if(this->attribute == "comment"){ files.at(i).tag()->setComment(values.at(i)); }
	else if(this->attribute == "genre"){ files.at(i).tag()->setGenre(values.at(i)); }
	else if(this->attribute == "year"){ files.at(i).tag()->setYear(std::atoi(values.at(i).toCString())); }
	else if(this->attribute == "track"){ files.at(i).tag()->setTrack(std::atoi(values.at(i).toCString())); }
}


//some simple getter functions
bool ME::Core::getErrorFlag(){ return errorFlag; }
std::vector <TagLib::FileRef> ME::Core::getFiles(){ return files; }
TagLib::FileRef ME::Core::getFileAt(int i){ return files.at(i); }
TagLib::String ME::Core::getCommand(){ return command; }
TagLib::String ME::Core::getAttribute(){ return attribute; }
std::vector <TagLib::String> ME::Core::getValues(){ return values; }
TagLib::String ME::Core::getValueAt(int i){ return values.at(i); }
std::vector <TagLib::String> ME::Core::getOutputs(){ return outputs; }
TagLib::String ME::Core::getOutputAt(int i){ return outputs.at(i); } 
