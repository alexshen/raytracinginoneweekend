#ifndef RAY_H
#define RAY_H

#include "vec.h"

template<typename T>
class basic_ray
{
public:
    basic_ray() = default;
    basic_ray(const T& origin, const T& dir)
        : origin(origin)
        , dir(dir)
    {
    }

    T point_at(float t) const
    {
        return origin + dir * t;
    }

    T origin;
    T dir;
};

using ray = basic_ray<vec3>;
using ray2 = basic_ray<vec2>;

#endif // RAY_H
