#ifndef UTILS_H
#define UTILS_H

#include "vec.h"
#include "ray.h"

vec3 random_in_unit_sphere();
vec2 random_in_unit_disk();
float random_unit();
float fast_rand();

struct aabb;
class sphere;

void expand(aabb& volume, const sphere& s);
bool intersect(const ray2& r, const aabb& volume);

#endif // UTILS_H
