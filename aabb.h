#ifndef AABB_H
#define AABB_H

#include "vec.h"
#include <algorithm>

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

    void expand(const aabb& rhs)
    {
        min.x() = std::min(min.x(), rhs.min.x());
        max.x() = std::max(max.x(), rhs.max.x());

        min.y() = std::min(min.y(), rhs.min.y());
        max.y() = std::max(max.y(), rhs.max.y());
    }

    vec2 min;
    vec2 max;
};

#endif /* AABB_H */
