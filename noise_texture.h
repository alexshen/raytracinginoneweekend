#ifndef NOISE_TEXTURE_H
#define NOISE_TEXTURE_H

#include "perlin.h"
#include "texture.h"
#include <cmath>

class noise_texture : public texture
{
public:
    noise_texture(float scale = 1.0f)
        : scale(scale)
    {
    }
    
    vec3 value(float u, float v, const vec3& p) const override
    {
        //return vec3::one * (perlin::noise(p * scale) + 1.0f) * 0.5f;
        //return vec3::one * (perlin::turb(p * scale) + 1.0f) * 0.5f;
        return vec3::one * 0.5f * (1.0f + std::sin(p.z() * scale + 6.0f * perlin::turb(p)));
    }
    
    float scale;
};

#endif // NOISE_TEXTURE_H
