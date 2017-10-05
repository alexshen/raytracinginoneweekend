#include "aabb.h"
#include <cmath>

bool aabb::overlap(const aabb& rhs) const
{
    for (int i = 0; i < 2; ++i) {
        float d = center[i] - rhs.center[i];
        if (std::abs(d) >= half_width + rhs.half_width) {
            return false;
        }
    }
    return true;
}
