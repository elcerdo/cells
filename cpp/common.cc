#include "common.h"

float random_uniform(float min, float max)
{
    return min+(max-min)*static_cast<float>(rand())/RAND_MAX;
}

Point::Point(int x, int y) : x(x), y(y) {}

Point Point::random(int width, int height) {
    return Point(rand()%width,rand()%height);
}

