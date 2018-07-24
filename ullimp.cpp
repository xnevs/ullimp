//
//  ullimp.cpp
//
//  Created by Jure Mihelič on 9/12/13.
//


#include <iostream>
#include <fstream>
#include <libgen.h>

#include "digraph.hpp"
#include "map.hpp"
#include "order.hpp"
#include "ullimp.hpp"


bool fileexists(const char *filename) {
    std::ifstream ifile(filename);
    return ifile.good();
}


int main(int argc, char * argv[]) {
	if (argc < 3) {
        std::cerr << "Not enough arguments.\n";
        std::cerr << "Usage: ullimp: <pattern> <target>\n";
        return 42;
    }
    if (!fileexists(argv[1])) { std::cerr << "Pattern file '" << argv[1] << "' not found." << std::endl; return 1; }
    if (!fileexists(argv[2])) { std::cerr << "Target file '" << argv[2] << "' not found." << std::endl; return 2; }
	Digraph g(argv[1]);
	Digraph h(argv[2]);
    
    UllImp alg(g, h);
	printf("%s ", basename(argv[1]));
    clock_t start = clock();
	alg.find(true);
    clock_t diffAll = clock() - start;
    
    // print statistics: count & times
    int time = (int) (diffAll * 1000 / (double) CLOCKS_PER_SEC);
    std::cout << alg.isoCount << " " << time << std::endl;
    
	return 0;
}
