#ifndef HITABLE_H
#define HITABLE_H

#include "vec.h"
#include "ray.h"
#include "material.h"
#include "object.h"
#include <vector>

struct hit_record
{
    float t;
    vec3 p;
    vec3 normal;
    float u, v;
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
    virtual bool hit(const ray3& r, float tmin, float tmax, hit_record& rec) const = 0;

    material_ptr mat;
};

using object_list = std::vector<object*>;
bool check_hit(const object_list& hitables, const ray3& r, float tmin, float tmax, hit_record& rec);

#endif // HITABLE_H
