#include "vec.h"

#include <cmath>
using namespace std;

bool refract(const vec3& r_in, const vec3& n, float ni_over_nt, vec3& refracted)
{
    // ni * sin(theta_i) = nt * sin(theta_t)
    // sin(theta_t) = ni/nt * sin(theta_i)
    //              = ni/nt * sqrt(1 - dot(r_in, n)^2)
    float dt = dot(r_in, n);
    float determinant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);
    if (determinant > 0) {
        refracted = ni_over_nt * r_in - (ni_over_nt * dt + sqrtf(determinant)) * n;
        return true;
    }
    return false;
}
