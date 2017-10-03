#include "sphere.h"
#include "ray.h"

sphere::sphere(const vec3& center, float radius, material_ptr mat)
    : hitable(std::move(mat))
    , center(center)
    , radius(radius)
{
}

bool sphere::hit(const ray& r, float tmin, float tmax, hit_record& rec) const
{
    vec3 oc = r.origin - center;
    float a = dot(r.dir, r.dir);
    float b = dot(r.dir, oc);
    float c = dot(oc, oc) - radius * radius;

    float discriminant = b * b - a * c;
    if (discriminant > 0) {
        float t = (-b - sqrt(discriminant)) / a;
        if (tmin < t && t < tmax) {
            rec = compute_hit(r, t);
            return true;
        }

        t = (-b + sqrt(discriminant)) / a;
        if (tmin < t && t < tmax) {
            rec = compute_hit(r, t);
            return true;
        }
    }
    return false;
}


hit_record sphere::compute_hit(const ray& r, float t) const
{
    hit_record rec;
    rec.t = t;
    rec.p = r.point_at(t);
    rec.normal = (rec.p - center) / radius;
    rec.mat = mat.get();
    return rec;
}
