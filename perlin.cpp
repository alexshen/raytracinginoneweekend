#include "perlin.h"
#include <algorithm>
#include <random>
using namespace std;

vec3 perlin::s_rand[sk_size];
int perlin::s_perm_x[sk_size];
int perlin::s_perm_y[sk_size];
int perlin::s_perm_z[sk_size];

static void permute(int* p, int n)
{
    iota(p, p + n, 0);
    random_device rd;
    ranlux24_base gen(rd());
    shuffle(p, p + n, gen);
}

float perlin::noise(const vec3& p)
{
    int ix = (int)std::floor(p.x());
    int iy = (int)std::floor(p.y());
    int iz = (int)std::floor(p.z());
    
    float u = smooth(p.x() - ix);
    float v = smooth(p.y() - iy);
    float w = smooth(p.z() - iz);
    
    float e[2][2][2];
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                auto dir = s_rand[s_perm_x[(ix + i) & sk_size_mask] ^
                                  s_perm_y[(iy + j) & sk_size_mask] ^
                                  s_perm_z[(iz + k) & sk_size_mask]];
                e[i][j][k] = dot(vec3(u - i, v - j, w - k), dir);
            }
        }
    }
    return tri_lerp(e, u, v, w);
}

float perlin::turb(const vec3& p, int depth)
{
    float sum = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    for (int i = 0; i < depth; ++i) {
        sum += noise(p * frequency) * amplitude;
        frequency *= 2.0f;
        amplitude *= 0.5f;
    }
    return sum;
}

void perlin::init()
{
    generate_n(s_rand, sk_size, []() {
        float x = 2 * random_unit() - 1;
        float y = 2 * random_unit() - 1;
        float z = 2 * random_unit() - 1;
        return vec3(x, y, z);
    });
    permute(s_perm_x, sk_size);
    permute(s_perm_y, sk_size);
    permute(s_perm_z, sk_size);
}
