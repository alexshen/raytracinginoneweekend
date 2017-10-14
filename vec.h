#ifndef VEC_H
#define VEC_H

#include <kismet/math/vector.h>

using vec3 = kismet::math::vector3f;
using vec2 = kismet::math::vector2f;

// calculate the refracted vector
// r_in, n must be unit vector
// return true if refraction occurs
bool refract(const vec3& r_in, const vec3& n, float ni_over_nt, vec3& refracted);

inline vec2 xz(const vec3& v)
{
	return vec2(v.x(), v.z());
}

#endif // VEC_H