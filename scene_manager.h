#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "partition.h"
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
    
    virtual void build_scene() = 0;
    
    spatial_partition& root() { return *m_root; }
    const spatial_partition& root() const { return *m_root; }
protected:
    std::vector<std::unique_ptr<object>> m_objects;
    std::unique_ptr<spatial_partition> m_root;
};

#endif /* SCENE_MANAGER_H */
