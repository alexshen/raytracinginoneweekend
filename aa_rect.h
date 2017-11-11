#ifndef AA_RECT_H
#define AA_RECT_H

#include "hitable.h"
#include "vec.h"
#include "aabb.h"
#include "utils.h"
#include <cmath>

class aa_rect : public hitable
{
public:
    aa_rect() = default;

    aa_rect(const vec2& min, const vec2& max, float k, int axis, material_ptr mat)
        : hitable(std::move(mat))
        , min(min)
        , max(max)
        , k(k)
        , axis(axis)
    {
        assert(axis >= 0 && axis < 3);
    }

    bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const override
    {
        // parallel or containment
        if (std::abs(r.dir[axis]) < 0.001f) { return false; }

        float t = (k - r.origin[axis]) / r.dir[axis];
        if (t < tmin || t > tmax) { return false; }

        int j = 0;
        for (int i = 0; i < 3; ++i) {
            if (i != axis) {
                float p = r.origin[i] + t * r.dir[i];
                if (p <= min[j] || p >= max[j]) {
                    return false;
                }
                rec.p[j] = p;
                ++j;
            }
        }
        rec.t = t;
        rec.u = inverse_lerp(min.x(), max.x(), rec.p[0]);
        rec.v = inverse_lerp(min.y(), max.y(), rec.p[1]);
        rec.mat = mat.get();
        return true;
    }

    aabb get_aabb() const override
    {
        vec2 mmin, mmax;
        switch (axis) {
        case 0:
            mmin.x() = mmax.x() = k;
            mmin.y() = min.y();
            mmax.y() = max.y();
            break;
        case 1:
            mmin = min;
            mmax = max;
            break;
        default:
            mmin.x() = min.x();
            mmax.x() = max.x();
            mmin.y() = mmax.y() = k;
            break;
        }
        return { mmin, mmax };
    }

    vec2 min; // min point of the aa rect in the plane perpendicular to the axis
    vec2 max; // max point of the aa rect in the plane perpendicular to the axis
    float k; // coordinate along the axis
    int axis;
};

#endif /* ifndef AA_RECT_H */
