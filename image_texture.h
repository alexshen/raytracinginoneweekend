#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "texture.h"
#include <memory>

class image_texture : public texture
{
public:
    image_texture(const unsigned char* data, int nx, int ny);
    
    image_texture(image_texture&) = delete;
    image_texture& operator =(image_texture&) = delete;
    
    vec3 value(float u, float v, const vec3& p) const override;
    
    static std::unique_ptr<image_texture> load(const char* path);
private:
    const unsigned char* m_data;
    int m_nx;
    int m_ny;
};

#endif /* IMAGE_TEXTURE_H */
