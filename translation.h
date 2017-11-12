#ifndef TRANSLATION_H
#define TRANSLATION_H

#include "hitable.h"
#include "vec.h"
#include "aabb.h"
#include <memory>
#include <utility>

class translation : public hitable
{
public:
    translation(std::unique_ptr<hitable> obj, const vec3& offset)
        : m_object(std::move(obj))
        , m_offset(offset)
    {
    }

    bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const override
    {
        // convert to local space
        ray lr(r.origin - m_offset, r.dir, r.time);
        if (m_object->hit(lr, tmin, tmax, rec)) {
            // convert to world space
            rec.p += m_offset;
            return true;
        }
        return false;
    }

    aabb get_aabb() const override
    {
        auto volume = m_object->get_aabb();
        volume.translate(xz(m_offset));
        return volume;
    }
private:
    std::unique_ptr<hitable> m_object;
    vec3 m_offset;
};

#endif /* TRANSLATION_H */
