#ifndef BLOCK_TEXTURE_H
#define BLOCK_TEXTURE_H

#include "texture.h"
#include <cassert>
#include <vector>

class block_texture : public texture
{
public:
    block_texture(const std::vector<vec3>& colors, int width)
        : m_colors(colors)
        , m_width(width)
        , m_height(colors.size() / width)
    {
    }
    
    vec3 value(float u, float v, const vec3& p) const override
    {
        auto x = ((int)(p.x() * 5) % m_width + m_width) % m_width;
        auto y = ((int)(p.z() * 5) % m_height + m_height) % m_height;
        return m_colors[y * m_width + x];
    }
private:
    std::vector<vec3> m_colors;
    int m_width;
    int m_height;
};

#endif // BLOCK_TEXTURE_H
