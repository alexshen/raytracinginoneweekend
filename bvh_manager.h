#ifndef BVH_MANAGER
#define BVH_MANAGER

#include "bvh_node.h"
#include "scene_manager.h"

#include <algorithm>
#include <iterator>
#include <memory>

class bvh_manager : public scene_manager
{
public:
    void build_scene() override
    {
        std::vector<object*> objects;
        objects.reserve(m_objects.size());
        std::transform(m_objects.begin(), m_objects.end(), 
                       std::back_inserter(objects), 
                       [](const auto& p) { return p.get(); });
        m_root = std::make_unique<bvh_node>(objects.data(), (int)objects.size());
    }
};

#endif