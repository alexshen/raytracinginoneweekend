#ifndef SPHERE_H
#define SPHERE_H

#include "aabb.h"
#include "hitable.h"
#include "vec.h"

class sphere : public hitable
{
public:
    sphere() = default;
    sphere(const vec3& center, float radius, material_ptr mat);

    bool hit(const ray3& r, float tmin, float tmax, hit_record& rec) const override;
    aabb3 get_aabb() const override;
public:
    vec3 center;
    float radius;
};

#endif // SPHERE_H
