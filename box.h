#ifndef BOX_H
#define BOX_H

#include "hitable.h"
#include "vec.h"
#include "aa_rect.h"
#include "aabb.h"
#include "flip_normal.h"
#include "hitable_list.h"
#include <memory>
#include <utility>
#include <numeric>

class box : public hitable
{
public:
    enum face { posz, negz, posy, negy, posx, negx };

    box(const vec3& min, const vec3& max, const material_ptr& mat)
    {
        m_hitables.add(aa_rect::xy(vec2(min.x(), min.y()), vec2(max.x(), max.y()), max.z(), mat));
        m_hitables.add(std::make_unique<flip_normal>(
            aa_rect::xy(vec2(min.x(), min.y()), vec2(max.x(), max.y()), min.z(), mat)));
        m_hitables.add(aa_rect::xz(vec2(min.x(), min.z()), vec2(max.x(), max.z()), max.y(), mat));
        m_hitables.add(std::make_unique<flip_normal>(
            aa_rect::xz(vec2(min.x(), min.z()), vec2(max.x(), max.z()), min.y(), mat)));
        m_hitables.add(aa_rect::yz(vec2(min.y(), min.z()), vec2(max.y(), max.z()), max.x(), mat));
        m_hitables.add(std::make_unique<flip_normal>(
            aa_rect::yz(vec2(min.y(), min.z()), vec2(max.y(), max.z()), min.x(), mat)));
    }

    bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const override
    {
		return m_hitables.hit(r, tmin, tmax, rec);
    }

    aabb get_aabb() const override
    {
		return m_hitables.get_aabb();
    }
private:
	hitable_list m_hitables;
};

#endif /* BOX_H */
