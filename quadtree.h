#ifndef QUADTREE_H
#define QUADTREE_H

#include "aabb.h"
#include "partition.h"
#include "ray.h"
#include "vec.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

class object;

class quadnode : public spatial_partition
{
    using object_list = std::vector<object*>;
public:
    static constexpr int child_count = 4;

    quadnode();
    quadnode(const aabb& volume, int depth = 1);

    quadnode(quadnode&& rhs);
    quadnode& operator =(quadnode&& rhs);

    void insert(object* p);
    // objs may contain duplicate objects
    void raycast(const ray2& r, std::vector<object*>& objs) const override;

    aabb& get_volume() { return m_volume; }
    const aabb& get_volume() const { return m_volume; }

    int get_depth() const { return m_depth; }

    quadnode* get_child(int i)
    {
        assert(i >= 0 && i < child_count);
        return m_child[i].get();
    }

    quadnode* get_child(int i) const
    {
        assert(i >= 0 && i < child_count);
        return m_child[i].get();
    }

    object_list::const_iterator objects_begin() const { return m_objects.begin(); }
    object_list::const_iterator objects_end() const { return m_objects.end(); }
    int object_count() const { return (int)m_objects.size(); }

    template<typename F>
    void visit(F&& visitor)
    {
        visit_impl<quadnode*>(std::forward<F>(visitor));
    }

    // make sure the visitor's parameter is const qualified
    template<typename F>
    auto visit(F&& visitor) const -> std::void_t<decltype(visitor(std::declval<const quadnode>()))>
    {
        visit_impl<const quadnode*>(std::forward<F>(visitor));
    }

    bool is_leaf() const { return !m_child[0]; }
private:
    template<typename T, typename F>
    void visit_impl(F&& visitor) const
    {
        visitor(*this);
        if (!is_leaf()) {
            for (const auto& c : m_child) {
                static_cast<T>(c.get())->visit(visitor);
            }
        }
    }

    void insert_impl(object* p);
    void raycast_impl(const ray2& r, std::vector<object*>& objs) const;

    static constexpr int max_depth = 10;
    static constexpr int max_objects = 10;

    std::unique_ptr<quadnode> m_child[child_count];

    aabb m_volume;
    int m_depth;

    object_list m_objects;
};

#endif // QUADTREE_H
