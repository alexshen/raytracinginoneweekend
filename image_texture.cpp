#include "image_texture.h"
#include "utils.h"
#include "stb_image.h"

image_texture::image_texture(const unsigned char* data, int nx, int ny)
    : m_data(data)
    , m_nx(nx)
    , m_ny(ny)
{
}

vec3 image_texture::value(float u, float v, const vec3& p) const
{
    int x = clamp<int>(u * m_nx, 0, m_nx - 1);
    int y = clamp<int>(v * m_ny, 0 ,m_ny - 1);
    auto color = &m_data[3 * m_nx * y + 3 * x];
    return vec3(color[0], color[1], color[2]) / 255.0f;
}

std::unique_ptr<image_texture> image_texture::load(const char* path)
{
    int nx, ny, n;
    auto p = stbi_load(path, &nx, &ny, &n, 3);
    if (!p) {
        return nullptr;
    }
    
    return std::make_unique<image_texture>(p, nx, ny);
}
