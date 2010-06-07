#include "minds.h"
#include "benchmark.h"

int main(int argc, char *argv[])
{
    PythonMinds::init();

    World world(300,300,15);
    world.addPlayer("dummy",0,mind_test1);
    world.addPlayer("spawner",0,mind_test2);
    if (PythonMinds::loadMind("player1","mind1")) { world.addPlayer("player1",0,PythonMinds::mind); } 
    if (PythonMinds::loadMind("player2","mind2")) { world.addPlayer("player2",0,PythonMinds::mind); } 

    benchmark(world);

    PythonMinds::destroy();

    return 0;
}

