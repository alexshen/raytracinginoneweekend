#ifndef PARTITION_H
#define PARTITION_H

#include "ray.h"
#include <vector>

class object;

class spatial_partition
{
public:
    virtual ~spatial_partition() = default;
    virtual void raycast(const ray3& r, std::vector<object*>& objs) const = 0;
};

#endif // PARTITION_H
