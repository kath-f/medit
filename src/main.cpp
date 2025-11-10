#include <iostream>

#include "ME.hpp"

int main(int argc, char** argv){
	ME::Core core;
	core.run(argc, argv);

	for(int i=0 ; i<core.getOutputs().size(); i++){
		std::cout << "\t" << core.getFileAt(i).file()->name() << ": "  << core.getOutputAt(i) << std::endl;
	}

	if(core.getErrorFlag()) return -1;
	return 0;
}
