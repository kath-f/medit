#pragma once

#include "ME.hpp"

#include <taglib/fileref.h>
#include <taglib/tstring.h>
#include <vector>

namespace ME{
	class Core{
	private:
		std::vector <TagLib::FileRef> files;
		TagLib::String command;
		TagLib::String attribute;
		std::vector <TagLib::String> values;
		std::vector <TagLib::String> outputs;

		bool errorFlag;
			
		void parse(int argc, char** argv);
		void checkForErrors();
		void runGet();
		void runSet();

		void populateTags(bool createTags, int i);
	public:
		Core(){};

		int main(int argc, char** argv);
		
		bool getErrorFlag();
		std::vector <TagLib::FileRef> getFiles();
		TagLib::FileRef getFileAt(int i);
		
		TagLib::String getCommand();
		
		TagLib::String getAttribute();
		
		std::vector <TagLib::String> getValues();
		TagLib::String getValueAt(int i);
		
		std::vector <TagLib::String> getOutputs();
		TagLib::String getOutputAt(int i);
	
		~Core(){};
	};
}

