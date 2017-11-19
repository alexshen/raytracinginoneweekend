#ifndef WORLD_H
#define WORLD_H

#include "hitable.h"
#include "aabb.h"

#include <memory>
#include <vector>

class hitable_list : public hitable
{
public:
    using hitable_ptr = std::unique_ptr<hitable>;

    hitable_list() = default;
    bool hit(const ray3& r, float tmin, float tmax, hit_record& rec) const override;
    void add(hitable_ptr obj);
	aabb3 get_aabb() const override { return m_aabb; }
private:
    std::vector<hitable_ptr> m_objects;
	aabb3 m_aabb;
};

#endif // WORLD_H
