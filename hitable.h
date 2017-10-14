#ifndef HITABLE_H
#define HITABLE_H

#include "vec.h"
#include "ray.h"
#include "material.h"
#include "object.h"

struct hit_record
{
    float t;
    vec3 p;
    vec3 normal;
    const material* mat;
};

class hitable : public object
{
public:
    hitable() = default;

    hitable(material_ptr mat)
        : mat(std::move(mat))
    {}

    virtual ~hitable() = default;
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const = 0;

    material_ptr mat;
};

#endif // HITABLE_H
