#define RANDOM_SCENE
#define SHOW_PROGRESS
#define MT

#include "camera.h"
#include "dielectric.h"
#include "lambertian.h"
#include "material.h"
#include "metal.h"
#include "quadtree.h"
#include "ray.h"
#include "scene_manager.h"
#include "sphere.h"
#include "sphere_object.h"
#include "utils.h"
#include "vec.h"
#include "world.h"

#include <ctpl_stl.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cmath>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <memory>
#include <mutex>
#include <random>
#include <unordered_set>
#include <vector>

using namespace std;
using namespace kismet;

using object_list = std::vector<object*>;

bool check_hit(const object_list& objects, const ray& r, float tmin, float tmax, hit_record& rec)
{
    bool hit = false;
    rec.t = tmax;
    hit_record tmp;
    for (int i = 0; i < objects.size(); ++i) {
        auto& obj = static_cast<hitable_object&>(*objects[i]);
        if (obj.get_object().hit(r, tmin, rec.t, tmp)) {
            rec = tmp;
            hit = true;
        }
    }
    return hit;
}

vec3 color(const ray& r, uint8_t& frame, scene_manager& scene, object_list& hitables, int depth = 0)
{
    hitables.clear();
    
    auto origin2 = vec2(r.origin.x(), r.origin.z());
    auto dir2 = vec2(r.dir.x(), r.dir.z());
    scene.root().raycast(ray2(origin2, dir2), hitables);

    hit_record rec;
    
    // 0.001 to avoid shadow acne
    if (check_hit(hitables, r, 0.001f, numeric_limits<float>::max(), rec)) {
        vec3 attenuation;
        ray scattered;
        if (depth < 50 && rec.mat->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, frame, scene, hitables, depth + 1);
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

scene_manager random_scene()
{
    scene_manager scene;

    scene.add(lambertian_sphere(vec3(0, -1000, 0), 1000, vec3::one * 0.5f));

    constexpr int x_count = 11;
    constexpr int y_count = 11;
    
    for (int a = -x_count; a < x_count; ++a) {
        for (int b = -y_count; b < y_count; ++b) {
            float choose_mat = random_unit();
            vec3 center(a + 0.9f * random_unit(), 0.2f, b + 0.9f * random_unit());
            if ((center - vec3(4, 0.2f, 0)).mag() > 0.9f) {
                if (choose_mat < 0.8f) {
                    scene.add(lambertian_sphere(center, 0.2f,
                                                  vec3(random_unit() * random_unit(),
                                                       random_unit() * random_unit(),
                                                       random_unit() * random_unit())));
                } else if (choose_mat < 0.95f) {
                    scene.add(metal_sphere(center, 0.2f,
                                             vec3(0.5f * (1 + random_unit()),
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

scene_manager simple_scene()
{
    scene_manager scene;
    
    scene.add(lambertian_sphere(vec3(0, 0, -1), 0.5f, vec3(0.1f, 0.2f, 0.5f)));
    scene.add(lambertian_sphere(vec3(0, -100.5, -1), 100, vec3(0.8f, 0.8f, 0)));
    scene.add(metal_sphere(vec3(1, 0, -1), 0.5f, vec3(0.8f, 0.6f, 0.2f)));
    scene.add(dielectric_sphere(vec3(-1, 0, -1), 0.5f, 1.5f));
    scene.add(dielectric_sphere(vec3(-1, 0, -1), -0.45f, 1.5f));
    
    return scene;
}


struct image
{
    int width;
    int height;
    int samples;
};

void render(scene_manager& scene, const camera& cam, object_list& hitables,
            const image& img, int start, int end, unsigned char* p)
{
    // start from 1, frame 0 is invalid
    uint8_t frame = 1;
    
    for (int i = start; i >= end; --i) {
        for (int j = 0; j < img.width; ++j) {
            vec3 c = vec3::zero;
            for (int n = 0; n < img.samples; ++n) {
                float u = (float)(j + random_unit()) / img.width;
                float v = (float)(i + random_unit()) / img.height;
                ray r = cam.get_ray(u, v);
                c += color(r, frame, scene, hitables);
            }
            c /= (float)img.samples;
            
            // gamma correction
            auto ir = (int)(sqrtf(c[0]) * 255.99f);
            auto ig = (int)(sqrtf(c[1]) * 255.99f);
            auto ib = (int)(sqrtf(c[2]) * 255.99f);
            
            *p++ = (unsigned char)ir;
            *p++ = (unsigned char)ig;
            *p++ = (unsigned char)ib;
        }
    }
}

void render(scene_manager& scene, const camera& cam, const image& img)
{
#ifdef MT
    int num_threads = 2;
    ctpl::thread_pool pool(num_threads);
#else
    int num_threads = 1;
#endif
    std::vector<unsigned char> buf(img.width * img.height * 3);
    std::vector<object_list> object_list_buf(num_threads);
    
#ifdef SHOW_PROGRESS
    mutex mtx;
    condition_variable cv;
    int counter = 0;
#endif
    
    const int rows = min(img.height / 2, 2);
    for (int i = img.height - 1; i >= 0; ) {
        int start = i;
        int end = max(i - rows + 1, 0);
#ifdef MT
        pool.push([&, start, end](int id) {
#else
            int id = 0;
#endif
            render(scene, cam, object_list_buf[id], img, start, end, &buf[(img.height - 1 - start) * img.width * 3]);
            
#ifdef SHOW_PROGRESS
            {
                unique_lock<mutex> lock(mtx);
                counter += start - end + 1;
                cv.notify_one();
            }
#endif
#ifdef MT
        });
#endif
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
    
#ifdef MT
    pool.stop(true);
#endif
    ofstream out("output.ppm");
    out << "P3\n" << img.width << " " << img.height << "\n255\n";
    for (auto i = 0u; i < buf.size(); i += 3) {
        out << (int)buf[i] << " " << (int)buf[i + 1] << " " << (int)buf[i + 2] << "\n";
    }
}

void dump(const quadnode& node)
{
    cout << setw(node.get_depth() - 1) << "*" << " " 
         << node.get_volume().center << ", " << node.get_volume().half_width;

    if (node.is_leaf()) {
        cout << "\n" << setw(node.get_depth());
        for (auto it = node.objects_begin(); it != node.objects_end(); ++it) {
            cout << " " << (*it)->id();
        }
    }
    cout << "\n";
}

int get_object_count(const quadnode& root)
{
    unordered_set<void*> objects;
    root.visit([&](const quadnode& node) {
        for (auto it = node.objects_begin(); it != node.objects_end(); ++it) {
            objects.insert(*it);
        }
    });
    return (int)objects.size();
}

int main(int argc, char* argv[])
{
    image img;
    img.width = 300;
    img.height = 150;
    img.samples = 100;
    if (argc > 1) {
        if (argc < 4) {
            cerr << "tracer width height samples\n";
            return 1;
        }

        img.width   = atoi(argv[1]);
        img.height  = atoi(argv[2]);
        img.samples = atoi(argv[3]);
    }

    vec3 pos, lookat;
    float aperture, dist_to_focus;
    
    scene_manager scene;
#ifndef RANDOM_SCENE
    pos = vec3(0, 2, 7);
    lookat = vec3::zero;
    aperture = 0;
    dist_to_focus = 1;
    
    scene = simple_scene();
#else
    pos = vec3(13, 2, 3);
    lookat = vec3::zero;
    aperture = 0.1f;
    dist_to_focus = 10.0f;
    
    scene = random_scene();
#endif

    scene.build_scene();
    
    camera cam(pos, lookat, vec3::up, 20.0f, (float)img.width / img.height, aperture, dist_to_focus);
    
    using namespace chrono;
    auto start = high_resolution_clock::now();

    render(scene, cam, img);

    auto end = high_resolution_clock::now();
    cout << "duration: " << duration_cast<milliseconds>(end - start).count() << "\n";
}
