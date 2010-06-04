#include "minds.h"
#include <ctime>

using std::endl;
using std::cout;

int main(int argc, char *argv[])
{
    cout.precision(1);

    World world(300,300);
    world.addPlayer("player1",0,mind_test1);
    world.addPlayer("player2",0,mind_test2);
    world.addPlayer("player3",0,mind_test1);
    world.addPlayer("player4",0,mind_test2);
    world.addPlayer("player5",0,mind_test1);
    world.addPlayer("player6",0,mind_test2);

    long long int nagents_total = 0;
    clock_t start = clock();
    for (int i=0; i<100000; i++) {
        world.tick();
        nagents_total += world.energies.size();
    }
    clock_t end = clock();
    world.print(cout);

    float elapsed = static_cast<float>(end-start)/CLOCKS_PER_SEC;
    cout<<endl;
    cout<<world.nticks<<" ticks in "<<std::fixed<<1e3*elapsed<<"ms"<<endl;
    cout<<std::fixed<<1e-3*world.nticks/elapsed<<" ticks/ms"<<endl;
    cout<<std::fixed<<static_cast<float>(nagents_total)/world.nticks<<" agents/ticks"<<endl;

    return 0;
}

