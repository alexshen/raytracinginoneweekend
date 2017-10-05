#include "scene_manager.h"

#include "sphere_object.h"
#include "vec.h"

#include <algorithm>

void expand(aabb& volume, const sphere& s)
{
    auto min = volume.min();
    auto max = volume.max();

    min.x() = std::min(min.x(), s.center.x() - s.radius);
    max.x() = std::max(max.x(), s.center.x() + s.radius);

    min.y() = std::min(min.y(), s.center.z() - s.radius);
    max.y() = std::max(max.y(), s.center.z() + s.radius);

    volume.center = (min + max) * 0.5f;

    auto size = max - min;
    volume.half_width = std::max(size[0], size[1]) * 0.5f;
}


void scene_manager::build_scene()
{
    aabb volume{vec2::zero, 0.0f};
    for (const auto& o : m_objects)
    {
        auto& hitable = static_cast<sphere_object*>(o.get())->get_object();
        expand(volume, static_cast<sphere&>(hitable));
    }
    
    m_root = std::make_unique<quadnode>(volume);
    for (const auto& o : m_objects)
    {
        m_root->insert(o.get());
    }
}
