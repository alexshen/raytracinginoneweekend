#ifndef AABB_H
#define AABB_H

#include "vec.h"

struct aabb
{
    vec2 center() const { return (min + max) * 0.5f; }
    vec2 extent() const { return (max - min) * 0.5f; }

    bool overlap(const aabb& rhs) const
    {
        return overlap(rhs, 0) && overlap(rhs, 1);
    }

    bool overlap(const aabb& rhs, int axis) const
    {
        float s = std::max(min[axis], rhs.min[axis]);
        float e = std::min(max[axis], rhs.max[axis]);
        return s < e;
    }

    vec2 min;
    vec2 max;
};

#endif /* AABB_H */
