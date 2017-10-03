#include "utils.h"
#include <random>
using namespace std;

vec3 random_in_unit_sphere()
{
    static uniform_real_distribution<float> s_dist;
    static ranlux24_base s_gen;

    vec3 d;
    do {
        // convert from [0,1) to (-1,1)
        d = 2.0f * vec3(s_dist(s_gen), s_dist(s_gen), s_dist(s_gen)) - vec3::one;
    } while (d.squared_mag() >= 1.0f);
    return d;
}

vec2 random_in_unit_disk()
{
    static uniform_real_distribution<float> s_dist;
    static ranlux24_base s_gen;

    vec2 d;
    do {
        // convert from [0,1) to (-1,1)
        d = 2.0f * vec2(s_dist(s_gen), s_dist(s_gen)) - vec2::one;
    } while (d.squared_mag() >= 1.0f);
    return d;
}

float random_unit()
{
    static uniform_real_distribution<float> s_dist;
    static ranlux24_base s_gen;
    return s_dist(s_gen);
}
