#define RANDOM_SCENE
#define SHOW_PROGRESS

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

#include <ctpl_stl.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
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

struct image
{
    int width;
    int height;
    int samples;
};

void render(const image& img, const world& w, const camera& cam, int start, int end, unsigned char* p)
{
    for (int i = start; i >= end; --i) {
        for (int j = 0; j < img.width; ++j) {
            vec3 c = vec3::zero;
            for (int n = 0; n < img.samples; ++n) {
                float u = (float)(j + random_unit()) / img.width;
                float v = (float)(i + random_unit()) / img.height;
                ray r = cam.get_ray(u, v);
                c += color(r, w);
            }
            c /= (float)img.samples;
            
            // gamma correction
            int ir = (int)(sqrtf(c[0]) * 255.99f);
            int ig = (int)(sqrtf(c[1]) * 255.99f);
            int ib = (int)(sqrtf(c[2]) * 255.99f);
            
            *p++ = (unsigned char)ir;
            *p++ = (unsigned char)ig;
            *p++ = (unsigned char)ib;
        }
    }
}

void render(const image& img, const world& w, const camera& cam)
{
    int num_threads = 2;
    ctpl::thread_pool pool(num_threads);
    std::vector<unsigned char> buf(img.width * img.height * 3);
    
#ifdef SHOW_PROGRESS
    mutex mtx;
    condition_variable cv;
    int counter = 0;
#endif
    
    const int rows = min(img.height / 2, 2);
    for (int i = img.height - 1; i >= 0; ) {
        int start = i;
        int end = max(i - rows + 1, 0);
        pool.push([&, start, end](auto) {
            render(img, w, cam, start, end, &buf[(img.height - 1 - start) * img.width * 3]);
            
#ifdef SHOW_PROGRESS
            {
                unique_lock<mutex> lock(mtx);
                counter += start - end + 1;
                cv.notify_one();
            }
#endif
        });
        i = --end;
    }
    
#ifdef SHOW_PROGRESS
    {
        unique_lock<mutex> lock(mtx);
        while (counter < img.height) {
            cout << "progress " << (int)((float)counter / img.height * 100) << "\n";
            cv.wait(lock);
        }
    }
    cout << "done\n";
#endif
    
    pool.stop(true);
    ofstream out("output.ppm");
    out << "P3\n" << img.width << " " << img.height << "\n255\n";
    for (auto c : buf) {
        out << (int)c << "\n";
    }
}

int main(int argc, char* argv[])
{
    image img;
    img.width = 300;
    img.height = 150;
    img.samples = 100;
    if (argc > 1)
    {
        if (argc < 4)
        {
            cerr << "tracer width height samples\n";
            return 1;
        }

        img.width   = atoi(argv[1]);
        img.height  = atoi(argv[2]);
        img.samples = atoi(argv[3]);
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

    camera cam(pos, lookat, vec3::up, 20.0f, (float)img.width / img.height, aperture, dist_to_focus);
    
    using namespace chrono;
    auto start = high_resolution_clock::now();
    render(img, w, cam);
    auto end = high_resolution_clock::now();
    cout << "duration: " << duration_cast<milliseconds>(end - start).count() << "\n";
}
