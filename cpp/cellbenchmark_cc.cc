#include "minds.h"
#include "benchmark.h"
#include <cstdlib>
#include <cstdio>

int main(int argc, char *argv[]) {
    srand(time(NULL));

    World world(500,500,50);
    world.addPlayer("playerdebug",0,new MindTest1);
    for (int i=0; i<49; i++) {
        char foo[256];
        sprintf(foo,"player%02d",i);
        world.addPlayer(std::string(foo),0,new MindTest3);
    }

    benchmark(world);

    return 0;
}

