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

void expand(aabb2& volume, const sphere& s)
{
    volume.min.x() = std::min(volume.min.x(), s.center.x() - s.radius);
    volume.max.x() = std::max(volume.max.x(), s.center.x() + s.radius);

    volume.min.y() = std::min(volume.min.y(), s.center.z() - s.radius);
    volume.max.y() = std::max(volume.max.y(), s.center.z() + s.radius);
}

static vec2 compute_sphere_uv(const vec3& p)
{
    using namespace kismet::math;
    
    float u = (std::atan2(p.x(), p.z()) + KISMET_PI_F) / (2.0f * KISMET_PI_F);
    float v = std::acos(p.y()) / KISMET_PI_F;
    
    return vec2(u, v);
}

static hit_record compute_sphere_hit(const vec3& center, float radius, const ray3& r, float t)
{
    hit_record rec;
    rec.t = t;
    rec.p = r.point_at(t);
    rec.normal = (rec.p - center) / radius;
    auto uv = compute_sphere_uv(rec.normal);
    rec.u = uv.x();
    rec.v = uv.y();
    return rec;
}

bool sphere_hit(const vec3& center, float radius, const ray3& r, float tmin, float tmax, hit_record& rec)
{
    vec3 oc = r.origin - center;
    float a = dot(r.dir, r.dir);
    float b = dot(r.dir, oc);
    float c = dot(oc, oc) - radius * radius;

    float discriminant = b * b - a * c;
    if (discriminant > 0) {
        float dis2 = sqrtf(discriminant);
        float inv_a = 1.0f / a;
        float t = (-b - dis2) * inv_a;
        if (tmin < t && t < tmax) {
            rec = compute_sphere_hit(center, radius, r, t);
            return true;
        }

        t = (-b + dis2) * inv_a;
        if (tmin < t && t < tmax) {
            rec = compute_sphere_hit(center, radius, r, t);
            return true;
        }
    }
    return false;
}

float tri_lerp(float e[2][2][2], float u, float v, float w)
{
    float res = 0;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                res += ((1 - i) * (1 - u) + i * u) *
                       ((1 - j) * (1 - v) + j * v) *
                       ((1 - k) * (1 - w) + k * w) * e[i][j][k];
            }
        }
    }
    return res;
}
