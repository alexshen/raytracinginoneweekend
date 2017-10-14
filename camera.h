#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"
#include "vec.h"

class camera
{
public:
    // fov in degress
    camera(const vec3& pos, const vec3& lookat, const vec3& up, float fov, float aspect,
           float aperture, float focus_distance, float time0, float time1);

    ray get_ray(float u, float v) const;

    vec3 origin;
    vec3 lower_left;
    vec3 horizontal;
    vec3 vertical;
    vec3 right;
    vec3 up;
    float lens_radius;
    float time0;
    float time1;
};

#endif // CAMERA_H
