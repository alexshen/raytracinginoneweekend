#ifndef UTILS_H
#define UTILS_H

#include "vec.h"
#include "ray.h"
#include "aabb.h"
#include <kismet/math/utility.h>
#include <cmath>
#include <utility>

vec3 random_in_unit_sphere();
vec2 random_in_unit_disk();
float random_unit();
float fast_rand();

struct aabb;
class sphere;
struct hit_record;

void expand(aabb2& volume, const sphere& s);

template<typename T>
bool intersect(const basic_ray<T>& r, const basic_aabb<T>& volume)
{
    constexpr float epsilon = 1e-6f;
    constexpr int size = std::tuple_size_v<T>;

    enum which_side : char { neg, pos, middle };

    // fast ray box intersection, Graphic Gems I
    char quadrant[size];
    // the edges to test against intersection
    float edges[size];
    float t[size];
    bool inside = true;

    // for each axis, find candidate edges
    for (int i = 0; i < size; ++i) {
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
        for (int i = 0; i < size; ++i) {
            if (quadrant[i] != middle && std::abs(r.dir[i]) > epsilon) {
                t[i] = (edges[i] - r.origin[i]) / r.dir[i];
            } else {
                // no intersection;
                t[i] = -1.0f;
            }
        }

        // find the intersection axis
        int axis = std::max_element(t, t + size) - t;
        if (t[axis] < 0) {
            return false;
        }

        // check if the intersection point is actually valid on other axes
        for (int i = 0; i < size; ++i) {
            if (i != axis) {
                float coord = r.origin[i] + t[axis] * r.dir[i];
                if (coord <= volume.min[i] || coord >= volume.max[i]) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool sphere_hit(const vec3& center, float radius, const ray3& r, float tmin, float tmax, hit_record& rec);

using kismet::math::lerp;
using kismet::math::inverse_lerp;
using kismet::math::clamp;
using kismet::math::deg2rad;

float tri_lerp(float e[2][2][2], float u, float v, float w);

inline float smooth(float x)
{
    return x * x * (3 - 2 * x);
}

inline auto sincos(float angle)
{
    float rad = deg2rad(angle);
    return std::make_pair(std::sin(rad), std::cos(rad));
}

inline float sincos(float angle, float& cosine)
{
    float rad = deg2rad(angle);
	cosine = std::cos(rad);
	return std::sin(rad);
}

#endif // UTILS_H
