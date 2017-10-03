#include "camera.h"
#include "dielectric.h"
#include "lambertian.h"
#include "material.h"
#include "metal.h"
#include "ray.h"
#include "sphere.h"
#include "utils.h"
#include "vec.h"
#include "world.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

using namespace std;
using namespace kismet;

vec3 color(const ray& r, const hitable& world, int depth = 0)
{
    hit_record rec;
    // 0.001 to avoid shadow acne
    if (world.hit(r, 0.001f, numeric_limits<float>::max(), rec)) {
        vec3 attenuation;
        ray scattered;
        if (depth < 50 && rec.mat->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, world, depth + 1);
        } else {
            return vec3::zero;
        }
    }

    auto dir = normalize(r.dir);
    float t = 0.5f * (dir.y() + 1.0f);
    return lerp(vec3(1.0f, 1.0f, 1.0f), vec3(0.5f, 0.7f, 1.0f), t);
}

inline unique_ptr<sphere> lambertian_sphere(const vec3& center, float r, const vec3& albedo)
{
    return make_unique<sphere>(center, r, make_shared<lambertian>(albedo));
}

inline unique_ptr<sphere> metal_sphere(const vec3& center, float r, const vec3& albedo, float fuzz = 0.0f)
{
    return make_unique<sphere>(center, r, make_shared<metal>(albedo, fuzz));
}

inline unique_ptr<sphere> dielectric_sphere(const vec3& center, float r, float index)
{
    return make_unique<sphere>(center, r, make_shared<dielectric>(index));
}

world random_scene()
{
    world scene;
    scene.add(lambertian_sphere(vec3(0, -1000, 0), 1000, vec3::one * 0.5f));
    for (int a = -11; a < 11; ++a) {
        for (int b = -11; b < 11; ++b) {
            float choose_mat = random_unit();
            vec3 center(a + 0.9f * random_unit(), 0.2f, b + 0.9f * random_unit());
            if ((center - vec3(4, 0.2f, 0)).mag() > 0.9f) {
                if (choose_mat < 0.8f) {
                    scene.add(lambertian_sphere(center, 0.2f, vec3(random_unit() * random_unit(),
                                                                   random_unit() * random_unit(),
                                                                   random_unit() * random_unit())));
                } else if (choose_mat < 0.95f) {
                    scene.add(metal_sphere(center, 0.2f, vec3(0.5f * (1 + random_unit()),
                                                              0.5f * (1 + random_unit()),
                                                              0.5f * (1 + random_unit())),
                                           0.5f * random_unit()));
                } else {
                    scene.add(dielectric_sphere(center, 0.2f, 1.5f));
                }
            }
        }
    }
    scene.add(dielectric_sphere(vec3(0, 1, 0), 1.0f, 1.5f));
    scene.add(lambertian_sphere(vec3(-4, 1, 0), 1.0f, vec3(0.4f, 0.2f, 0.1f)));
    scene.add(metal_sphere(vec3(4, 1, 0), 1.0f, vec3(0.7f, 0.6f, 0.5f)));
    return scene;
}

int main(int argc, char* argv[])
{
    int width = 300;
    int height = 150;
    int samples = 100;
    if (argc > 1)
    {
        if (argc < 4)
        {
            cerr << "tracer width height samples\n";
            return 1;
        }

        width = atoi(argv[1]);
        height = atoi(argv[2]);
        samples = atoi(argv[3]);
    }

    vec3 pos, lookat;
    float aperture, dist_to_focus;
    world w;
#ifndef RANDOM_SCENE
    w.add(lambertian_sphere(vec3(0, 0, -1), 0.5f, vec3(0.1f, 0.2f, 0.5f)));
    w.add(lambertian_sphere(vec3(0, -100.5, -1), 100, vec3(0.8f, 0.8f, 0)));
    w.add(metal_sphere(vec3(1, 0, -1), 0.5f, vec3(0.8f, 0.6f, 0.2f)));
    w.add(dielectric_sphere(vec3(-1, 0, -1), 0.5f, 1.5f));
    w.add(dielectric_sphere(vec3(-1, 0, -1), -0.45f, 1.5f));

    pos = vec3(3, 3, 2);
    lookat = vec3(0, 0, -1);
    aperture = 2.0f;
    dist_to_focus = (lookat - pos).mag();
#else
    w = random_scene();

    pos = vec3(13, 2, 3);
    lookat = vec3::zero;
    aperture = 0.1f;
    dist_to_focus = 10.0f;
#endif

    camera cam(pos, lookat, vec3::up, 20.0f, (float)width / height, aperture, dist_to_focus);

    ofstream out("output.ppm");
    out << "P3\n" << width << " " << height << "\n255\n";
    for (int i = height - 1; i >= 0; --i) {
        for (int j = 0; j < width; ++j) {
            vec3 c = vec3::zero;
            for (int n = 0; n < samples; ++n) {
                float u = (float)(j + random_unit()) / width;
                float v = (float)(i + random_unit()) / height;
                ray r = cam.get_ray(u, v);
                c += color(r, w);
            }
            c /= (float)samples;
#ifdef GAMMA
            int ir = (int)(sqrt(c[0]) * 255.99f);
            int ig = (int)(sqrt(c[1]) * 255.99f);
            int ib = (int)(sqrt(c[2]) * 255.99f);
#else
            int ir = (int)(c[0] * 255.99f);
            int ig = (int)(c[1] * 255.99f);
            int ib = (int)(c[2] * 255.99f);
#endif

            out << ir << " " << ig << " " << ib << "\n";
        }
    }
}
