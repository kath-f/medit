#pragma once

#include "ME.hpp"

#include <string>

namespace ME{
	class File{
	private:
		std::string name;
		FileType type;
	public:
		File(std::string path_to_file);
		~File(){};

		std::string getName();
		FileType getType();

		std::string getTypeName();
	};
}
