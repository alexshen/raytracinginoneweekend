#ifndef SPHERE_OBJECT_H
#define SPHERE_OBJECT_H

#include "hitable_object.h"
#include "sphere.h"
#include <cmath>

class sphere_object : public hitable_object
{
public:
    sphere_object(std::unique_ptr<sphere> p)
        : hitable_object(std::move(p))
    {
    }

    aabb get_aabb() const override
    {
        const auto& obj = static_cast<const sphere&>(get_object());
        return aabb{vec2(obj.center.x(), obj.center.z()), obj.radius};
    }
};

#endif // SPHERE_OBJECT_H
