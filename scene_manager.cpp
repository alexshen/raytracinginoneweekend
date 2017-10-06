#include "scene_manager.h"

#include "sphere_object.h"
#include "vec.h"

#include <algorithm>

void expand(aabb& volume, const sphere& s)
{
    volume.min.x() = std::min(volume.min.x(), s.center.x() - s.radius);
    volume.max.x() = std::max(volume.max.x(), s.center.x() + s.radius);

    volume.min.y() = std::min(volume.min.y(), s.center.z() - s.radius);
    volume.max.y() = std::max(volume.max.y(), s.center.z() + s.radius);
}


void scene_manager::build_scene()
{
    aabb volume{ vec2::zero, vec2::zero };
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
