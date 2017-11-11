#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec.h"
#include "ray.h"

#include <memory>
#include <utility>

struct hit_record;

class material
{
public:
    virtual ~material() = default;
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation,
                         ray& scattered) const = 0;
    virtual vec3 emitted(float u, float v, const vec3& p) const { return vec3::zero; }
};

using material_ptr = std::shared_ptr<material>;

#endif // MATERIAL_H
