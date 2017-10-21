#ifndef CHECKER_TEXTURE_H
#define CHECKER_TEXTURE_H

#include "texture.h"
#include <cmath>
#include <utility>

class checker_texture : public texture
{
public:
    checker_texture(texture_ptr odd, texture_ptr even)
        : odd(std::move(odd))
        , even(std::move(even))
    {
    }
    
    vec3 value(float u, float v, const vec3& p) const override
    {
        float sine = std::sin(p.x() * 10.0f) * std::sin(p.y() * 10.0f) * std::sin(p.z() * 10.0f);
        if (sine < 0) {
            return odd->value(u, v, p);
        } else {
            return even->value(u, v, p);
        }
    }
    
    texture_ptr odd;
    texture_ptr even;
};

#endif /* CHECKER_TEXTURE_H */
