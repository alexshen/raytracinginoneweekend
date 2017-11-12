#ifndef UTILS_H
#define UTILS_H

#include "vec.h"
#include "ray.h"
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

void expand(aabb& volume, const sphere& s);
bool intersect(const ray2& r, const aabb& volume);
bool sphere_hit(const vec3& center, float radius, const ray& r, float tmin, float tmax, hit_record& rec);

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
