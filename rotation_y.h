#ifndef ROTATION_Y_H
#define ROTATION_Y_H

#include "hitable.h"
#include "utils.h"
#include "aabb.h"
#include "mat.h"
#include <memory>
#include <utility>
#include <cmath>

class rotation_y : public hitable
{
public:
    rotation_y(std::unique_ptr<hitable> obj, float angle)
        : m_object(std::move(obj))
        , m_angle(angle)
    {
    }

    bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const override
    {
        // convert to local space
        auto m2local = mat3_rotate_y(-m_angle);

        ray lr;
        lr.origin = m2local * r.origin;
        lr.dir = m2local * r.dir;
        lr.time = r.time;

        if (m_object->hit(lr, tmin, tmax, rec)) {
            auto m2world = transpose(m2local);
            rec.p = m2world * rec.p;
            // assume no scale only rotation
            rec.normal = m2world * rec.normal;
            return true;
        }
        return false;
    }

    aabb get_aabb() const override
    {
        auto volume = m_object->get_aabb();
        volume.rotate(m_angle);
        return volume;
    }
private:
    std::unique_ptr<hitable> m_object;
    float m_angle;
};

#endif /* ROTATION_Y_H */
