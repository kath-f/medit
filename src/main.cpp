#include "ME.hpp"
#include "ME_Parser.hpp"

#include <iostream>

int main(int argc, char** argv){
	
	ME::Parser parsed_input(argc, argv);
	for(int i=0; i<parsed_input.getInputFile().size(); i++){
		std::cout << "Input " << i << ": ";
		std::cout << parsed_input.getInputFile().at(i).getName() << " ";
		std::cout << parsed_input.getInputFile().at(i).getTypeName() << std::endl;
	}

	std::cout << "Command: " << parsed_input.getCommand() << std::endl;
	std::cout << "Attribute: " << parsed_input.getAttribute() << std::endl;

	for(int i=0; i<parsed_input.getValue().size(); i++){
		std::cout << "Value: " << i << ": ";
		std::cout << parsed_input.getValue().at(i) << std::endl;
	}

	return 0;
}
