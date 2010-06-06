#include "benchmark.h"

#include <ctime>
#include <iostream>

using std::endl;
using std::cout;

void benchmark(World &world, int niters)
{
    cout.precision(1);

    long long int nagents_total = 0;
    clock_t start = clock();
    for (int i=0; i<niters; i++) {
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
}

