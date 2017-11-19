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
    void raycast(const ray3& r, std::vector<object*>& objs) const override;

    const aabb3& get_aabb() const { return m_volume; }
    bool is_leaf() const { return !m_left; }
private:
    static constexpr int max_objects = 2;
    static constexpr int bin_num = 1024;

    std::unique_ptr<bvh_node> m_left;
    std::unique_ptr<bvh_node> m_right;
    std::vector<object*> m_objects;

    aabb3 m_volume;
};

#endif // BVH_NODE_H
