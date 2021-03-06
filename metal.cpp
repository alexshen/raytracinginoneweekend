#include "metal.h"
#include "ray.h"
#include "hitable.h"
#include "utils.h"

bool metal::scatter(const ray3& r_in, const hit_record& rec, vec3& attenuation, ray3& scattered) const
{
    vec3 reflected = reflect(r_in.dir, rec.normal);
    scattered = ray3(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time);
    attenuation = albedo;
    return dot(scattered.dir, rec.normal) > 0;
}
