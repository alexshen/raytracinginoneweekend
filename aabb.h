#ifndef AABB_H
#define AABB_H

#include "vec.h"

struct aabb
{
    vec2 center;
    float half_width;

    vec2 min() const { return vec2(center.x() - half_width, center.y() - half_width); }
    vec2 max() const { return vec2(center.x() + half_width, center.y() + half_width); }
    
    bool overlap(const aabb& rhs) const;
};

#endif /* AABB_H */
