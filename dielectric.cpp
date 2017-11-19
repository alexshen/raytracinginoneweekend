#include "dielectric.h"

#include "ray.h"
#include "hitable.h"
#include "utils.h"

#include <cmath>
using namespace std;

static float schlick(float cosine, float n)
{
    float r0 = (1 - n) / (1 + n);
    r0 *= r0;
    return r0 + (1.0f - r0) * pow(1.0f - cosine, 5);
}

bool dielectric::scatter(const ray3& r_in, const hit_record& rec, vec3& attenuation,
                         ray3& scattered) const
{
    vec3 uin = normalize(r_in.dir);
    attenuation = vec3::one;

    float ni_over_nt;
    float cosine = dot(uin, rec.normal);
    vec3 normal;
    if (cosine > 0) {
        ni_over_nt = index;
        normal = -rec.normal;
    } else {
        ni_over_nt = 1.0f / index;
        normal = rec.normal;
    }

    float reflect_prob;
    vec3 refracted;
    if (refract(uin, normal, ni_over_nt, refracted)) {
        // if > 0, then we assume that light travels from denser material to air
        if (cosine > 0) {
            cosine = sqrtf(1.0f - index * index * (1.0f - cosine * cosine));
        } else {
            cosine = -cosine;
        }
        reflect_prob = schlick(cosine, index);
    } else {
        reflect_prob = 1.0f;
    }

    if (random_unit() < reflect_prob) {
        scattered = ray3(rec.p, reflect(uin, normal), r_in.time);
    } else {
        scattered = ray3(rec.p, refracted, r_in.time);
    }
    return true;
}
