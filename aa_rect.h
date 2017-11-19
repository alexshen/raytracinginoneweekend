#ifndef AA_RECT_H
#define AA_RECT_H

#include "hitable.h"
#include "vec.h"
#include "aabb.h"
#include "utils.h"
#include <cmath>
#include <memory>

class aa_rect : public hitable
{
public:
    enum axis { x, y, z };

    aa_rect() = default;

    aa_rect(const vec2& min, const vec2& max, float k, axis ax, material_ptr mat)
        : hitable(std::move(mat))
        , min(min)
        , max(max)
        , k(k)
        , ax(ax)
    {

    }

    bool hit(const ray3& r, float tmin, float tmax, hit_record& rec) const override
    {
        // parallel or containment
        if (std::abs(r.dir[ax]) < 0.001f) { return false; }

        float t = (k - r.origin[ax]) / r.dir[ax];
        if (t < tmin || t > tmax) { return false; }

        int j = 0;
        for (int i = 0; i < 3; ++i) {
            if (i != ax) {
                float p = r.origin[i] + t * r.dir[i];
                if (p <= min[j] || p >= max[j]) {
                    return false;
                }
                rec.p[i] = p;
                ++j;
            } else {
                rec.p[i] = k;
            }
        }
        rec.t = t;
        rec.u = inverse_lerp(min.x(), max.x(), rec.p[0]);
        rec.v = inverse_lerp(min.y(), max.y(), rec.p[1]);
        rec.normal = vec3::zero;
        rec.normal[ax] = 1.0f;
        rec.mat = mat.get();
        return true;
    }

    aabb3 get_aabb() const override
    {
        switch (ax) {
        case 0: return { vec3(k, min[0], min[1]), vec3(k, max[0], max[1]) };
        case 1: return { vec3(min[0], k, min[1]), vec3(max[0], k, max[1]) };
        default: return { vec3(min[0], min[1], k), vec3(max[0], max[1], k) };
        }
    }

    static std::unique_ptr<aa_rect> xy(const vec2& min, const vec2& max, float k, material_ptr mat)
    {
        return std::make_unique<aa_rect>(min, max, k, axis::z, mat);
    }

    static std::unique_ptr<aa_rect> yz(const vec2& min, const vec2& max, float k, material_ptr mat)
    {
        return std::make_unique<aa_rect>(min, max, k, axis::x, mat);
    }

    static std::unique_ptr<aa_rect> xz(const vec2& min, const vec2& max, float k, material_ptr mat)
    {
        return std::make_unique<aa_rect>(min, max, k, axis::y, mat);
    }

    vec2 min; // min point of the aa rect
    vec2 max; // max point of the aa rect
    float k; // coordinate along the axis
    axis ax;
};

#endif /* ifndef AA_RECT_H */
