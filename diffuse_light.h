#ifndef DIFFUSE_LIGHT_H
#define DIFFUSE_LIGHT_H

#include "material.h"
#include "texture.h"
#include <utility>

class diffuse_light : public material
{
public:
    diffuse_light(texture_ptr p)
        : m_emit(std::move(p))
    {}

    bool scatter(const ray3& r_in, const hit_record& rec, vec3& attenuation,
                 ray3& scattered) const override
    { return false; }

    vec3 emitted(float u, float v, const vec3& p) const override
    {
        return m_emit->value(u, v, p);
    }
private:
    texture_ptr m_emit;
};

#endif /* ifndef DIFFUSE_LIGHT_H */
