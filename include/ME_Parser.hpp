#pragma once

#include "ME.hpp"
#include "ME_File.hpp"

#include <vector>
#include <string>
#include <array>

namespace ME{
class Parser{
private:
	std::vector <File> input_file;
	std::string command;
	std::string attribute;
	std::vector <std::string> value;
public:
	Parser(int argc, char** argv);
	~Parser(){}

	std::vector <File> getInputFile();
	std::string getCommand();
	std::string getAttribute();
	std::vector <std::string> getValue();
};
}
