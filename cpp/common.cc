#include "common.h"

#include <cstdlib>

int signum(int x)
{
    if (x == 0) return 0;
    if (x > 0) return 1;
    return -1;
}

float random_uniform(float min, float max)
{
    return min+(max-min)*static_cast<float>(rand())/RAND_MAX;
}

Point::Point(int x, int y) : x(x), y(y) {}

Point Point::random(int width, int height)
{
    return Point(rand()%width,rand()%height);
}

Point Point::getNewPositionToDestination(const Point &dest) const
{
    return Point(x+signum(dest.x-x),y+signum(dest.y-y));
}

Point Point::left() const { return Point(x-1,y); }
Point Point::right() const { return Point(x+1,y); }
Point Point::down() const { return Point(x,y-1); }
Point Point::top() const { return Point(x,y+1); }
