#ifndef CONSTANT_TEXTURE_H
#define CONSTANT_TEXTURE_H
              
#include "texture.h"

class constant_texture : public texture
{
public:
    constant_texture(const vec3& c)
        : color(c)
    {}
    
    vec3 value(float u, float v, const vec3& p) const override
    {
        return color;
    }

    vec3 color;
};

#endif /* ifndef CONSNTAT_TEXTURE_H */
