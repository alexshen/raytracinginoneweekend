#ifndef PERLIN_H
#define PERLIN_H

#include "vec.h"
#include "utils.h"
#include <cmath>
#include <vector>

class perlin
{
public:
    // returns value in [-1,1]
    static float noise(const vec3& p);
    static float turb(const vec3& p, int depth = 7);
    static void init();
private:
    static constexpr int sk_size = 256;
    static constexpr int sk_size_mask = sk_size - 1;
    
    static vec3 s_rand[sk_size];
    static int s_perm_x[sk_size];
    static int s_perm_y[sk_size];
    static int s_perm_z[sk_size];
};

#endif // PERLIN_H
