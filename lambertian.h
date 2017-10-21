#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "material.h"
#include "vec.h"
#include "texture.h"
#include <utility>

class lambertian : public material
{
public:
    lambertian() = default;
    lambertian(texture_ptr a)
        : albedo(std::move(a))
    {
    }

    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation,
                 ray& scattered) const override;

    texture_ptr albedo;
};

#endif // LAMBERTIAN_H
