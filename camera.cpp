#include "camera.h"
#include "utils.h"

#include <kismet/math/math_trait.h>

#include <cmath>

using namespace std;
using namespace kismet::math;

camera::camera(const vec3& pos, const vec3& lookat, const vec3& up, float fov, float aspect,
               float aperture, float focus_dist)
    : origin(pos)
    , lens_radius(aperture * 0.5f)
{
    vec3 cam_forward = normalize(lookat - pos);
    right = normalize(cross(cam_forward, up));
    this->up = cross(right, cam_forward);

    float half_height = tan(radians(fov) * 0.5f);
    float half_width = aspect * half_height;
    lower_left = origin + cam_forward * focus_dist - half_width * focus_dist * right
        - half_height * focus_dist * this->up;
    horizontal = 2.0f * half_width * focus_dist * right;
    vertical = 2.0f * half_height * focus_dist * this->up;
}

ray camera::get_ray(float u, float v) const
{
    vec2 rd = random_in_unit_disk() * lens_radius;
    vec3 offset = right * rd.x() + up * rd.y();

    ray r;
    r.origin = origin + offset;
    r.dir = lower_left + u * horizontal + v * vertical - r.origin;

    return r;
}
