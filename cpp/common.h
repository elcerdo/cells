#ifndef __COMMON_H__
#define __COMMON_H__

#include <cassert>

float random_uniform(float min=0, float max=1);

struct Point {
    Point(int x=0, int y=0);
    Point getNewPositionToDestination(const Point &dest) const;
    Point left() const;
    Point right() const;
    Point down() const;
    Point top() const;
    static Point random(int width, int height);
    int x,y;
};

template <typename T>
struct Map {
    Map(int width, int height) : width(width), height(height), size(width*height) {
        flat = new T[size];
    }
    ~Map() {
        delete [] flat;
    }

    bool isValid(int x, int y) const {
        return x>=0 and x<width and y>=0 and y<height; 
    }

    bool isValid(const Point &point) const { return isValid(point.x,point.y); }

    T &get(int x, int y) {
        assert(isValid(x,y));
        return flat[x*height+y];
    }

    T &get(const Point &point) { return get(point.x,point.y); }

    const T &get(int x, int y) const {
        assert(isValid(x,y));
        return flat[x*height+y];
    }
    
    const T &get(const Point &point) const { return get(point.x,point.y); }

    const int width,height,size;
    T *flat;
};
typedef Map<float> MapFloat;

template <typename T>
struct PositionLess {
    bool operator()(const T &a, const T &b) {
        if (a.position.x != b.position.x) return a.position.x < b.position.x;
        else return a.position.y < b.position.y;
    }
};

#endif
