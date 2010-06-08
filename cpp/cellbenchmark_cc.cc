#include "minds.h"
#include "benchmark.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));

    World world(50,50,15);
    world.addPlayer("player1",0,mind_test1);
    world.addPlayer("player2",0,mind_test2);
    world.addPlayer("player3",0,mind_test1);
    world.addPlayer("player4",0,mind_test2);
    world.addPlayer("player5",0,mind_test1);
    world.addPlayer("player6",0,mind_test2);

    benchmark(world);

    return 0;
}

