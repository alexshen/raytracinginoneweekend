#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "partition.h"

#include <memory>
#include <utility>
#include <vector>

class scene_manager
{
public:
    object* get_object(int i) { return m_objects[i].get(); }
    int object_count() const { return (int)m_objects.size(); }
    
    void add(std::unique_ptr<object> p)
    {
        m_objects.push_back(std::move(p));
    }

	template<typename T, typename... Args>
	void add(Args&&... args)
	{
		add(make_unique<T>(std::forward<Args>(args)...));
	}
    
    virtual void build_scene() = 0;
    
    spatial_partition& root() { return *m_root; }
    const spatial_partition& root() const { return *m_root; }
protected:
    std::vector<std::unique_ptr<object>> m_objects;
    std::unique_ptr<spatial_partition> m_root;
};

#endif /* SCENE_MANAGER_H */
