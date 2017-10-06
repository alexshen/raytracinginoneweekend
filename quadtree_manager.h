#ifndef QUADTREE_MANAGER_H
#define QUADTREE_MANAGER_H

#include "vec.h"
#include "quadtree.h"
#include "scene_manager.h"
#include "sphere_object.h"
#include "utils.h"

#include <memory>
#include <utility>
 
class quadtree_manager : public scene_manager
{
public:
    void build_scene() override
    {
        aabb volume{ vec2::zero, vec2::zero };
        for (const auto& o : m_objects)
        {
            auto& hitable = static_cast<sphere_object*>(o.get())->get_object();
            expand(volume, static_cast<sphere&>(hitable));
        }

        auto root = std::make_unique<quadnode>(volume);
        for (const auto& o : m_objects)
        {
            root->insert(o.get());
        }
        m_root = std::move(root);
    }
};

#endif