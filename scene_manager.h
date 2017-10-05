#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "quadtree.h"

#include "sphere.h"
#include "sphere_object.h"

#include <memory>
#include <utility>
#include <vector>

class scene_manager
{
public:
    object* get_object(int i) { return m_objects[i].get(); }
    int object_count() const { return (int)m_objects.size(); }
    
    void add(std::unique_ptr<sphere> p)
    {
        m_objects.push_back(std::make_unique<sphere_object>(std::move(p)));
    }
    
    void build_scene();
    
    quadnode& root() { return *m_root; }
    const quadnode& root() const { return *m_root; }
private:
    std::vector<std::unique_ptr<object>> m_objects;
    std::unique_ptr<quadnode> m_root;
};

#endif /* SCENE_MANAGER_H */
