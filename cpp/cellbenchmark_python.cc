#include "mindspython.h"
#include "benchmark.h"
#include <sstream>
#include <iostream>
#include <cstdlib>

using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
    srand(time(NULL));
    Python::init();

    World world(300,300,15);
    for (int k=1; k<argc; k++) {
        std::stringstream ss;
        ss<<"player"<<k;
        if (Python::loadMind(ss.str(),argv[k])) {
            cout<<"loaded "<<argv[k]<<" for "<<ss.str()<<endl;
            world.addPlayer(ss.str(),0,new Python::MindPython(ss.str()));
        } 
    }

    benchmark(world,200);

    Python::destroy();

    return 0;
}

