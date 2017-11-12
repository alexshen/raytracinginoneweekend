#ifndef BOX_H
#define BOX_H

#include "hitable.h"
#include "vec.h"
#include "aa_rect.h"
#include "aabb.h"
#include "flip_normal.h"
#include <memory>
#include <utility>

class box : public hitable
{
public:
    enum face { posz, negz, posy, negy, posx, negx };

    box(const vec3& min, const vec3& max, const material_ptr& mat)
    {
        m_faces[posz] = aa_rect::xy(vec2(min.x(), min.y()), vec2(max.x(), max.y()), max.z(), mat);
        m_faces[negz] = std::make_unique<flip_normal>(
            aa_rect::xy(vec2(min.x(), min.y()), vec2(max.x(), max.y()), min.z(), mat));
        m_faces[posy] = aa_rect::xz(vec2(min.x(), min.z()), vec2(max.x(), max.z()), max.y(), mat);
        m_faces[negy] = std::make_unique<flip_normal>(
            aa_rect::xz(vec2(min.x(), min.z()), vec2(max.x(), max.z()), max.y(), mat));
        m_faces[posx] = aa_rect::yz(vec2(min.y(), min.z()), vec2(max.y(), max.z()), max.x(), mat);
        m_faces[negx] = std::make_unique<flip_normal>(
            aa_rect::yz(vec2(min.y(), min.z()), vec2(max.y(), max.z()), max.x(), mat));
    }

    bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const override
    {
        for (const auto& p : m_faces) {
            if (p->hit(r, tmin, tmax, rec)) {
                return true;
            }
        }
        return false;
    }

    aabb get_aabb() const override
    {
        return { vec2(aa_rect_k(negx), aa_rect_k(negz)),
                 vec2(aa_rect_k(posx), aa_rect_k(posz)) };
    }
private:
    float aa_rect_k(face f) const
    {
        return static_cast<aa_rect*>(m_faces[f].get())->k;
    }
    // +z, -z, +y, -y, +x, -x
    std::unique_ptr<hitable> m_faces[6];
};

#endif /* BOX_H */
