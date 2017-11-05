#ifndef UTILS_H
#define UTILS_H

#include "vec.h"
#include "ray.h"
#include <kismet/math/utility.h>

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

float tri_lerp(float e[2][2][2], float u, float v, float w);

inline float smooth(float x)
{
    return x * x * (3 - 2 * x);
}

#endif // UTILS_H
