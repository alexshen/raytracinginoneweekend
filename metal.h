#ifndef METAL_H
#define METAL_H 

#include "material.h"
#include "vec.h"

class metal : public material
{
public:
    metal() = default;
    metal(const vec3& a, float fuzz = 0)
        : albedo(a)
        , fuzz(fuzz)
    { }

    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation,
                 ray& scattered) const override;

    vec3 albedo;
    float fuzz;
};

#endif // METAL_H
