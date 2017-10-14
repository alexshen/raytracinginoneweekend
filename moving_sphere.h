#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H

#include "hitable.h"
#include "vec.h"
#include "ray.h"

class moving_sphere : public hitable
{
public:
    moving_sphere() = default;
    moving_sphere(const vec3& center0, const vec3& center1, float time0, float time1, float radius, material_ptr mat);
    
    bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const override;
    aabb get_aabb() const override;
    
    vec3 center0;
    vec3 center1;
    float time0;
    float time1;
    float radius;
};

#endif /* MOVING_SPHERE_H */
