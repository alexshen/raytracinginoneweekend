#ifndef WORLD_H
#define WORLD_H

#include "hitable.h"

#include <memory>
#include <vector>

class world : public hitable
{
public:
    using hitable_ptr = std::unique_ptr<hitable>;

    world() = default;
    bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const override;
    void add(hitable_ptr p);
private:
    std::vector<hitable_ptr> m_objects;
};

#endif // WORLD_H
