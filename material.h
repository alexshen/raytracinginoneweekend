#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec.h"

#include <memory>
#include <utility>

class ray;
struct hit_record;

class material
{
public:
    virtual ~material() {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation,
                         ray& scattered) const = 0;
};

using material_ptr = std::shared_ptr<material>;

#endif // MATERIAL_H
