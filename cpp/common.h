#ifndef __COMMON_H__
#define __COMMON_H__

#include <cstdlib>
#include <cassert>

float random_uniform(float min=0, float max=1);

struct Point
{
    Point(int x=0, int y=0);
    Point getNewPositionToDestination(const Point &dest) const;
    static Point random(int width, int height);
    int x,y;
};

template <typename T>
struct Map
{
    Map(int width, int height) : width(width), height(height), size(width*height), flat(NULL) {
        flat = new T[size];
    }
    ~Map() {
        delete [] flat;
    }

    T &get(int x, int y) {
        assert(x>=0 and x<width);
        assert(y>=0 and y<height);
        return flat[x*height+y];
    }

    T &get(const Point &point) { return get(point.x,point.y); }

    const T &get(int x, int y) const {
        assert(x>=0 and x<width);
        assert(y>=0 and y<height);
        return flat[x*height+y];
    }
    
    const T &get(const Point &point) const { return get(point.x,point.y); }

    const int width,height,size;
    T *flat;
};

#endif
