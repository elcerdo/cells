#include "minds.h"
#include "benchmark.h"
#include <cstdlib>
#include <cstdio>

int main(int argc, char *argv[]) {
    srand(time(NULL));

    World world(500,500,50);
    for (int i=0; i<50; i++) {
        char foo[256];
        sprintf(foo,"player%02d",i);
        world.addPlayer(std::string(foo),0,mind_test3);
    }

    benchmark(world);

    return 0;
}

