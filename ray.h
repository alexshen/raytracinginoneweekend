#ifndef RAY_H
#define RAY_H

#include "vec.h"

class ray
{
public:
    ray() = default;
    ray(const vec3& origin, const vec3& dir)
        : origin(origin)
        , dir(dir)
    {
    }

    vec3 point_at(float t) const
    {
        return origin + dir * t;
    }

    vec3 origin;
    vec3 dir;
};

#endif // RAY_H
