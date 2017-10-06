#define FAST_RAND
//#define DRAND48

#include "utils.h"
#include "aabb.h"
#include "sphere.h"

#include <cmath>
#ifndef FAST_RAND
#  include <random>
#endif

vec3 random_in_unit_sphere()
{
    vec3 d;
    do {
        float x = random_unit();
        float y = random_unit();
        float z = random_unit();
        // convert from [0,1) to (-1,1)
        d = 2.0f * vec3(x, y, z) - vec3::one;
    } while (d.squared_mag() >= 1.0f);
    return d;
}

vec2 random_in_unit_disk()
{
    vec2 d;
    do {
        float x = random_unit();
        float y = random_unit();
        // convert from [0,1) to (-1,1)
        d = 2.0f * vec2(x, y) - vec2::one;
    } while (d.squared_mag() >= 1.0f);
    return d;
}

float random_unit()
{
#ifndef FAST_RAND
    static uniform_real_distribution<float> s_dist;
    static ranlux24_base s_gen;
    return s_dist(s_gen);
#else
    return fast_rand();
#endif
}

float fast_rand()
{
#ifdef DRAND48
    return (float)drand48();
#else
    // see https://software.intel.com/en-us/articles/fast-random-number-generator-on-the-intel-pentiumr-4-processor/
    constexpr unsigned int rand_max = 0x8000;
    static thread_local unsigned int s_seed;
    s_seed = 214013u * s_seed + 2531011u;
    auto r = (s_seed >> 16) & (rand_max - 1);
    return (float)r / rand_max;
#endif
}

void expand(aabb& volume, const sphere& s)
{
    volume.min.x() = std::min(volume.min.x(), s.center.x() - s.radius);
    volume.max.x() = std::max(volume.max.x(), s.center.x() + s.radius);

    volume.min.y() = std::min(volume.min.y(), s.center.z() - s.radius);
    volume.max.y() = std::max(volume.max.y(), s.center.z() + s.radius);
}

bool intersect(const ray2 & r, const aabb & volume)
{
    constexpr float epsilon = 1e-6f;

    enum which_side : char { neg, pos, middle };

    // fast ray box intersection, Graphic Gems I
    char quadrant[2];
    // the edges to test against intersection
    float edges[2];
    float t[2];
    bool inside = true;

    // for each axis, find candidate edges
    for (int i = 0; i < 2; ++i) {
        if (r.origin[i] < volume.min[i]) {
            quadrant[i] = neg;
            edges[i] = volume.min[i];
            inside = false;
        } else if (r.origin[i] > volume.max[i]) {
            quadrant[i] = pos;
            edges[i] = volume.max[i];
            inside = false;
        } else {
            quadrant[i] = middle;
        }
    }

    // origin is outside of the bounding volume, check intersection
    if (!inside) {
        for (int i = 0; i < 2; ++i) {
            if (quadrant[i] != middle && std::abs(r.dir[i]) > epsilon) {
                t[i] = (edges[i] - r.origin[i]) / r.dir[i];
            } else {
                // no intersection;
                t[i] = -1.0f;
            }
        }

        // find the intersection axis
        int axis = 0;
        if (t[0] < t[1]) {
            axis = 1;
        }

        if (t[axis] < 0) {
            return false;
        }

        // check if the intersection point is actually valid on another axis
        int check_axis = 1 - axis;
        float coord = r.origin[check_axis] + t[axis] * r.dir[check_axis];
        if (coord <= volume.min[check_axis] || coord >= volume.max[check_axis]) {
            return false;
        }
    }

    return true;
}
