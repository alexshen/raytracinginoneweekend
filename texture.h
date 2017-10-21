#ifndef TEXTURE_H
#define TEXTURE_H

#include "vec.h"
#include <memory>

class texture
{
public:
    virtual ~texture() = default;
    virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

using texture_ptr = std::shared_ptr<texture>;

#endif /* ifndef TEXTURE_H */
