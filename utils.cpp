#define FAST_RAND

#include "utils.h"
#ifndef FAST_RAND
#  include <random>
using namespace std;
#endif

vec3 random_in_unit_sphere()
{
#ifndef FAST_RAND
    static uniform_real_distribution<float> s_dist;
    static ranlux24_base s_gen;
#endif

    vec3 d;
    do {
#ifndef FAST_RAND
        float x = s_dist(s_gen);
        float y = s_dist(s_gen);
        float z = s_dist(s_gen);
#else
        float x = fast_rand();
        float y = fast_rand();
        float z = fast_rand();
#endif
        // convert from [0,1) to (-1,1)
        d = 2.0f * vec3(x, y, z) - vec3::one;
    } while (d.squared_mag() >= 1.0f);
    return d;
}

vec2 random_in_unit_disk()
{
#ifndef FAST_RAND
    static uniform_real_distribution<float> s_dist;
    static ranlux24_base s_gen;
#endif

    vec2 d;
    do {
#ifndef FAST_RAND
        float x = s_dist(s_gen);
        float y = s_dist(s_gen);
#else
        float x = fast_rand();
        float y = fast_rand();
#endif
        // convert from [0,1) to (-1,1)
        d = 2.0f * vec2(x, y) - vec2::one;
    } while (d.squared_mag() >= 1.0f);
    return d;
}

float random_unit()
{
#ifndef FAST_RAND
    static uniform_real_distribution<float> s_dist;
    static ranlux24_base s_gen;
    return s_dist(s_gen);
#else
    return fast_rand();
#endif
}

float fast_rand()
{
    // see https://software.intel.com/en-us/articles/fast-random-number-generator-on-the-intel-pentiumr-4-processor/
    constexpr unsigned int rand_max = 0x8000;
    static unsigned int s_seed;
    s_seed = 214013u * s_seed + 2531011u;
    auto r = (s_seed >> 16) & (rand_max - 1);
    return (float)r / rand_max;
}
