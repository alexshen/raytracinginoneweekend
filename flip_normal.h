#ifndef FLIP_NORMAL_H
#define FLIP_NORMAL_H

#include "hitable.h"
#include "aabb.h"
#include <memory>
#include <utility>

class flip_normal : public hitable
{
public:
    flip_normal(std::unique_ptr<hitable> obj)
        : m_object(std::move(obj))
    {}

    bool hit(const ray3& r, float tmin, float tmax, hit_record& rec) const override
    {
        if (m_object->hit(r, tmin, tmax, rec))
        {
            rec.normal = -rec.normal;
            return true;
        }
        return false;
    }

    aabb3 get_aabb() const override
    {
        return m_object->get_aabb();
    }
private:
    std::unique_ptr<hitable> m_object;
};

#endif /* FLIP_NORMAL_H */
