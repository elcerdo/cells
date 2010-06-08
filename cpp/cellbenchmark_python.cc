#include "minds.h"
#include "benchmark.h"
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    PythonMinds::init();

    World world(300,300,15);
    for (int k=1; k<argc; k++) {
        std::stringstream ss;
        ss<<"player"<<k;
        if (PythonMinds::loadMind(ss.str(),argv[k])) {
            cout<<"loaded "<<argv[k]<<" for "<<ss.str()<<endl;
            world.addPlayer(ss.str(),0,PythonMinds::mind);
        } 
    }

    benchmark(world,100);

    PythonMinds::destroy();

    return 0;
}

