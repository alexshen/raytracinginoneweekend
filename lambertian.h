#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "material.h"
#include "vec.h"

class lambertian : public material
{
public:
    lambertian() = default;
    lambertian(const vec3& a)
        : albedo(a)
    {
    }

    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation,
                 ray& scattered) const override;

    vec3 albedo;
};

#endif // LAMBERTIAN_H
