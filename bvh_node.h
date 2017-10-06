#ifndef BVH_NODE_H
#define BVH_NODE_H

#include "aabb.h"
#include "partition.h"

#include <memory>
#include <vector>

class bvh_node : public spatial_partition
{
public:
    bvh_node(object** objs, int n);
    void raycast(const ray2 & r, std::vector<object*>& objs) const override;

    const aabb& get_aabb() const { return m_volume; }
    bool is_leaf() const { return !m_left; }
private:
    static constexpr int max_objects = 10;

    std::unique_ptr<bvh_node> m_left;
    std::unique_ptr<bvh_node> m_right;
    std::vector<object*> m_objects;

    aabb m_volume;
};

#endif // BVH_NODE_H