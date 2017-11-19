#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hitable.h"
#include "texture.h"
#include "isotropic.h"
#include "aabb.h"
#include <memory>
#include <utility>

class constant_medium : public hitable
{
public:
    constant_medium(std::unique_ptr<hitable> boundary,
                    float density,
                    texture_ptr albedo)
        : m_boundary(std::move(boundary))
        , m_density(density)
        , m_phase_function(std::make_unique<isotropic>(std::move(albedo)))
    {
    }

    bool hit(const ray3& r, float tmin, float tmax, hit_record& rec) const override;

    aabb3 get_aabb() const override
    {
        return m_boundary->get_aabb();
    }
private:
    std::unique_ptr<hitable> m_boundary;
    float m_density;
    std::unique_ptr<material> m_phase_function;
};

#endif /* CONSTANT_MEDIUM_H */
