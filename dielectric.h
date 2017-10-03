#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "material.h"

class dielectric : public material
{
public:
    dielectric(float index = 1.0f)
        : index(index)
    {}

    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation,
                 ray& scattered) const override;
    float index;
};

#endif /* ifndef DIELECTRIC_H */
