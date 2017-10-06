#include "bvh_node.h"
#include "object.h"
#include "utils.h"

#include <iterator>
#include <algorithm>

using namespace std;

bvh_node::bvh_node(object** objs, int n)
{
    if (n <= max_objects) {
        m_objects.assign(objs, objs + n);

        m_volume = m_objects[0]->get_aabb();
        for (auto i = 1u; i < m_objects.size(); ++i) {
            m_volume.expand(m_objects[i]->get_aabb());
        }
    } else {
        auto axis = (int)(2.0f * random_unit());
        sort(objs, objs + n, [axis](auto lhs, auto rhs) {
            return lhs->get_aabb().center()[axis] < rhs->get_aabb().center()[axis];
        });

        m_left = make_unique<bvh_node>(objs, n / 2);
        m_right = make_unique<bvh_node>(objs + n / 2, n - n / 2);

        m_volume = m_left->get_aabb();
        m_volume.expand(m_right->get_aabb());
    }
}

void bvh_node::raycast(const ray2 & r, std::vector<object*>& objs) const
{
    if (!intersect(r, m_volume)) {
        return;
    }

    if (is_leaf()) {
        copy(m_objects.begin(), m_objects.end(), back_inserter(objs));
    } else {
        m_left->raycast(r, objs);
        m_right->raycast(r, objs);
    }
}
