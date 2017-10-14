#include "sphere.h"
#include "ray.h"
#include "utils.h"

sphere::sphere(const vec3& center, float radius, material_ptr mat)
    : hitable(std::move(mat))
    , center(center)
    , radius(radius)
{
}

bool sphere::hit(const ray& r, float tmin, float tmax, hit_record& rec) const
{
    if (sphere_hit(center, radius, r, tmin, tmax, rec)) {
        rec.mat = mat.get();
        return true;
    }
    return false;
}

aabb sphere::get_aabb() const
{
    return { xz(center), radius };
}
