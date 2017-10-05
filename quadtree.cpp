#include "quadtree.h"

#include "object.h"

#include <algorithm>
#include <cassert>
#include <utility>
using namespace std;

quadnode::quadnode()
    : quadnode({vec2::zero, 0.0f}, 1)
{
}

quadnode::quadnode(const aabb& volume, int depth)
    : m_volume(volume)
    , m_depth(depth)
{
    assert(depth >= 1 && depth <= max_depth);
}

quadnode::quadnode(quadnode&& rhs)
    : m_volume(rhs.m_volume)
    , m_depth(rhs.m_depth)
    , m_objects(move(rhs.m_objects))
{
    for (int i = 0; i < child_count; ++i) {
        m_child[i] = move(rhs.m_child[i]);
    }
}

quadnode& quadnode::operator =(quadnode&& rhs)
{
    if (&rhs != this) {
        for (int i = 0; i < child_count; ++i) {
            m_child[i] = move(rhs.m_child[i]);
        }
        
        m_objects = move(rhs.m_objects);
        m_volume = rhs.m_volume;
        m_depth = rhs.m_depth;
    }
    return *this;
}

void quadnode::insert(object* p)
{
    assert(p);
    insert_impl(p);
}

void quadnode::insert_impl(object* p)
{
    auto volume = p->get_aabb();
    
    if (!volume.overlap(m_volume)) {
        return;
    }

    bool straddled = false;
    int index = 0;
    // check if object is straddling cells
    for (int i = 0; i < 2; ++i) {
        float d = volume.center[i] - m_volume.center[i];
        if (abs(d) < volume.half_width) {
            straddled = true;
            break;
        }
        if (d < 0.0f) {
            index |= 1 << i;
        }
    }
    
    if (straddled || m_depth == max_depth) {
        m_objects.push_back(p);
        return;
    }

    if (is_leaf()) {
        for (int i = 0; i < child_count; ++i) {
            float child_hw = m_volume.half_width * 0.5f;
            float dx = (i & 1) ? -child_hw : child_hw;
            float dy = (i & 2) ? -child_hw : child_hw;

            m_child[i] = make_unique<quadnode>(
                aabb{ m_volume.center + vec2(dx, dy), child_hw },
                m_depth + 1);
        }
    }

    m_child[index]->insert_impl(p);
}

void quadnode::raycast(const ray2& r, std::vector<object*>& objs) const
{
    assert(objs.empty());
    raycast_impl(r, objs);
}

void quadnode::raycast_impl(const ray2& r, std::vector<object*>& objs) const
{
    constexpr float epsilon = 0.0001f;

    auto min = m_volume.min();
    auto max = m_volume.max();

    enum which_side : char { neg, pos, middle };

    // fast ray box intersection, Graphic Gems I
    char quadrant[2];
    // the edges to test agianst intersection
    float edges[2];
    float t[2];
    bool inside = true;

    // for each axis, find candidate edges
    for (int i = 0; i < 2; ++i) {
        if (r.origin[i] < min[i]) {
            quadrant[i] = neg;
            edges[i] = min[i];
            inside = false;
        } else if (r.origin[i] > max[i]) {
            quadrant[i] = pos;
            edges[i] = max[i];
            inside = false;
        } else {
            quadrant[i] = middle;
        }
    }

    // origin is outside of the bounding volume, check intersection
    if (!inside) {
        for (int i = 0; i < 2; ++i) {
            if (quadrant[i] != middle && abs(r.dir[i]) > epsilon) {
                t[i] = (edges[i] - r.origin[i]) / r.dir[i];
            } else {
                // no intersection;
                t[i] = -1.0f;
            }
        }

        // find the intersection axis
        int axis = 0;
        if (t[0] < t[1]) {
            axis = 1;
        }

        if (t[axis] < 0) {
            return;
        }

        // check if the intersection point is actually valid on another axis
        int check_axis = 1 - axis;
        float coord = r.origin[check_axis] + t[axis] * r.dir[check_axis];
        if (coord <= min[check_axis] || coord >= max[check_axis]) {
            return;
        }
    }

    for (const auto& o : m_objects) {
        objs.push_back(o);
    }
    if (!is_leaf()) {
        for (const auto& c : m_child) {
            c->raycast_impl(r, objs);
        }
    }
}
