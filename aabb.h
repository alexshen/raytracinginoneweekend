#ifndef AABB_H
#define AABB_H

#include "vec.h"
#include "utils.h"
#include "mat.h"
#include <algorithm>
#include <cmath>

struct aabb
{
    aabb() = default;
    aabb(const vec2& min, const vec2& max)
        : min(min)
        , max(max)
    {
    }
    
    aabb(const vec2& center, float radius)
        : min(center.x() - radius, center.y() - radius)
        , max(center.x() + radius, center.y() + radius)
    {
    }
    
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

    void translate(const vec2& offset)
    {
        min += offset;
        max += offset;
    }

    void rotate(float angle)
    {
        auto mmin = vec2::zero;
        auto mmax = vec2::zero;
        auto m = mat2_rotate(angle);
        // for each axis
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                float a = min[j] * m[i][j];
                float b = max[j] * m[i][j];
                mmin[j] += std::min(a, b);
                mmax[j] += std::max(a, b);
            }
        }
        min = mmin;
        max = mmax;
    }

    vec2 min;
    vec2 max;
};

#endif /* AABB_H */
