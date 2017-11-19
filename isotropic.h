#ifndef ISOTROPIC_H
#define ISOTROPIC_H

#include "hitable.h"
#include "texture.h"
#include "utils.h"
#include <utility>

class isotropic : public material
{
public:
    isotropic(texture_ptr tex)
        : m_albedo(std::move(tex))
    { }

    bool scatter(const ray3& r_in, const hit_record& rec, vec3& attenuation,
                 ray3& scattered) const override
    {
        attenuation = m_albedo->value(rec.u, rec.v, rec.p);
        scattered = ray3(rec.p, random_in_unit_sphere(), r_in.time);
        return true;
    }
private:
    texture_ptr m_albedo;
};

#endif /* ISOTROPIC_H */
