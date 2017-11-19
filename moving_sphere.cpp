#include "moving_sphere.h"
#include "aabb.h"
#include "utils.h"
#include <utility>

moving_sphere::moving_sphere(const vec3& center0, const vec3& center1,
                             float time0, float time1, float radius, material_ptr mat)
    : hitable(std::move(mat))
    , center0(center0)
    , center1(center1)
    , time0(time0)
    , time1(time1)
    , radius(radius)
{
}

bool moving_sphere::hit(const ray3& r, float tmin, float tmax, hit_record& rec) const
{
    float t = inverse_lerp(time0, time1, r.time);
    vec3 center = lerp(center0, center1, t);
    if (sphere_hit(center, radius, r, tmin, tmax, rec)) {
        rec.mat = mat.get();
        return true;
    }
    return false;
}

aabb3 moving_sphere::get_aabb() const
{
    aabb3 s1(center0, radius);
    s1.expand({center1, radius});
    return s1;
}
