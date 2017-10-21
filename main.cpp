#define RANDOM_SCENE
#define MT

#include "camera.h"
#include "constant_texture.h"
#include "checker_texture.h"
#include "dielectric.h"
#include "lambertian.h"
#include "material.h"
#include "metal.h"
#include "quadtree.h"
#include "ray.h"
#include "scene_manager.h"
#include "quadtree_manager.h"
#include "bvh_manager.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "utils.h"
#include "vec.h"
#include "world.h"

#include <ctpl_stl.h>
#include <boost/program_options.hpp>

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
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

using namespace std;
using namespace kismet;
namespace po = boost::program_options;

using object_list = std::vector<object*>;

bool check_hit(const object_list& hitables, const ray& r, float tmin, float tmax, hit_record& rec)
{
    bool hit = false;
    rec.t = tmax;
    hit_record tmp;
    for (auto o : hitables) {
        auto& obj = static_cast<hitable&>(*o);
        if (obj.hit(r, tmin, rec.t, tmp)) {
            rec = tmp;
            hit = true;
        }
    }
    return hit;
}

vec3 color(const ray& r, scene_manager& scene, object_list& hitables, int depth = 0)
{
    hitables.clear();
    
    auto origin2 = xz(r.origin);
    auto dir2 = xz(r.dir);
    scene.root().raycast(ray2(origin2, dir2, r.time), hitables);

    hit_record rec;
    
    // 0.001 to avoid shadow acne
    if (check_hit(hitables, r, 0.001f, numeric_limits<float>::max(), rec)) {
        vec3 attenuation;
        ray scattered;
        if (depth < 50 && rec.mat->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, scene, hitables, depth + 1);
        } else {
            return vec3::zero;
        }
    }

    auto dir = normalize(r.dir);
    float t = 0.5f * (dir.y() + 1.0f);
    return lerp(vec3(1.0f, 1.0f, 1.0f), vec3(0.5f, 0.7f, 1.0f), t);
}

inline texture_ptr texture_constant(const vec3& albedo)
{
    return make_shared<constant_texture>(albedo);
}

inline unique_ptr<hitable> lambertian_sphere(const vec3& center, float r, const vec3& albedo)
{
    return make_unique<sphere>(center, r, make_shared<lambertian>(texture_constant(albedo)));
}

inline unique_ptr<hitable> lambertian_sphere(const vec3& center, float r, texture_ptr albedo)
{
    return make_unique<sphere>(center, r, make_shared<lambertian>(move(albedo)));
}

inline unique_ptr<hitable> lambertian_moving_sphere(const vec3& center0, const vec3& center1,
                                                    float time0, float time1,
                                                    float radius, const vec3& albedo)
{
    return make_unique<moving_sphere>(center0, center1,
                                      time0, time1,
                                      radius,
                                      make_shared<lambertian>(texture_constant(albedo)));
}

inline unique_ptr<hitable> metal_sphere(const vec3& center, float r, const vec3& albedo, float fuzz = 0.0f)
{
    return make_unique<sphere>(center, r, make_shared<metal>(albedo, fuzz));
}

inline unique_ptr<hitable> dielectric_sphere(const vec3& center, float r, float index)
{
    return make_unique<sphere>(center, r, make_shared<dielectric>(index));
}

struct options
{
    string output;
    int num_threads;
    bool report_progress;
};

unique_ptr<scene_manager> random_scene(bool quadtree)
{
    unique_ptr<scene_manager> scene;
    if (quadtree) {
        scene = make_unique<quadtree_manager>();
    } else {
        scene = make_unique<bvh_manager>();
    }

    auto checker_tex = make_shared<checker_texture>(texture_constant(vec3(0.2f, 0.3f, 0.1f)),
                                                    texture_constant(vec3::one * 0.9f));
    scene->add(lambertian_sphere(vec3(0, -1000, 0), 1000, move(checker_tex)));

    constexpr int x_count = 11;
    constexpr int y_count = 11;
    
    for (int a = -x_count; a < x_count; ++a) {
        for (int b = -y_count; b < y_count; ++b) {
            float choose_mat = random_unit();
            vec3 center(a + 0.9f * random_unit(), 0.2f, b + 0.9f * random_unit());
            if ((center - vec3(4, 0.2f, 0)).mag() > 0.9f) {
                if (choose_mat < 0.8f) {
                    scene->add(lambertian_moving_sphere(center, center + vec3(0.0f, 0.5f * random_unit(), 0.0f),
                                                        0.0f, 1.0f, 0.2f,
                                                        vec3(random_unit() * random_unit(),
                                                             random_unit() * random_unit(),
                                                             random_unit() * random_unit())));
                } else if (choose_mat < 0.95f) {
                    scene->add(metal_sphere(center, 0.2f,
                                             vec3(0.5f * (1 + random_unit()),
                                                  0.5f * (1 + random_unit()),
                                                  0.5f * (1 + random_unit())),
                                             0.5f * random_unit()));
                } else {
                    scene->add(dielectric_sphere(center, 0.2f, 1.5f));
                }
            }
        }
    }

    scene->add(dielectric_sphere(vec3(0, 1, 0), 1.0f, 1.5f));
    scene->add(lambertian_sphere(vec3(-4, 1, 0), 1.0f, vec3(0.4f, 0.2f, 0.1f)));
    scene->add(metal_sphere(vec3(4, 1, 0), 1.0f, vec3(0.7f, 0.6f, 0.5f)));
    
    return scene;
}

unique_ptr<scene_manager> simple_scene(bool quadtree)
{
    unique_ptr<scene_manager> scene;
    
    if (quadtree) {
        scene = make_unique<quadtree_manager>();
    } else {
        scene = make_unique<bvh_manager>();
    }

    scene->add(lambertian_sphere(vec3(0, 0, -1), 0.5f, vec3(0.1f, 0.2f, 0.5f)));
    scene->add(lambertian_sphere(vec3(0, -100.5, -1), 100, vec3(0.8f, 0.8f, 0)));
    scene->add(metal_sphere(vec3(1, 0, -1), 0.5f, vec3(0.8f, 0.6f, 0.2f)));
    scene->add(dielectric_sphere(vec3(-1, 0, -1), 0.5f, 1.5f));
    scene->add(dielectric_sphere(vec3(-1, 0, -1), -0.45f, 1.5f));
    
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
    for (int i = start; i >= end; --i) {
        for (int j = 0; j < img.width; ++j) {
            vec3 c = vec3::zero;
            for (int n = 0; n < img.samples; ++n) {
                float u = (float)(j + random_unit()) / img.width;
                float v = (float)(i + random_unit()) / img.height;
                ray r = cam.get_ray(u, v);
                c += color(r, scene, hitables);
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

void render(scene_manager& scene, const camera& cam, const image& img, const options& opts)
{
#ifdef MT
    int num_threads = opts.num_threads;
    ctpl::thread_pool pool(num_threads);
#else
    int num_threads = 1;
#endif
    std::vector<unsigned char> buf(img.width * img.height * 3);
    std::vector<object_list> object_list_buf(num_threads);
    
    mutex mtx;
    condition_variable cv;
    int counter = 0;
    
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
            
            if (opts.report_progress) {
                unique_lock<mutex> lock(mtx);
                counter += start - end + 1;
                cv.notify_one();
            }
#ifdef MT
        });
#endif
        i = --end;
    }
    
    if (opts.report_progress) {
        unique_lock<mutex> lock(mtx);
        while (counter < img.height) {
            cout << "progress " << (int)((float)counter / img.height * 100) << "\n";
            cv.wait(lock);
        }
        cout << "done\n";
    }
    
#ifdef MT
    pool.stop(true);
#endif
    ofstream out(opts.output);
    out << "P3\n" << img.width << " " << img.height << "\n255\n";
    for (auto i = 0u; i < buf.size(); i += 3) {
        out << (int)buf[i] << " " << (int)buf[i + 1] << " " << (int)buf[i + 2] << "\n";
    }
}

void dump(const quadnode& node)
{
    cout << setw(node.get_depth() - 1) << "*" << " "
        << node.get_volume().center() << ", " << node.get_volume().extent();

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
    options opts;
    bool use_quadtree;
    
    po::options_description desc;
    desc.add_options()
        ("help,h", "help message")
        (",j", po::value<int>(&opts.num_threads)->default_value(thread::hardware_concurrency()), "threads for rendering")
        ("width", po::value<int>(&img.width)->default_value(300), "image width")
        ("height", po::value<int>(&img.height)->default_value(200), "image height")
        ("sample", po::value<int>(&img.samples)->default_value(100), "samples per pixel")
        ("quadtree", po::bool_switch(&use_quadtree)->default_value(false), "use quadtree for spatial partitioning")
        (",p", po::bool_switch(&opts.report_progress)->default_value(true), "report rendering progress")
        ("output,o", po::value<string>(&opts.output)->default_value("output.ppm"), "output ppm name")
    ;
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (const po::error& e) {
        cerr << e.what() << "\n";
        return 1;
    }
    
    if (vm.count("help") || vm.count("h")) {
        cerr << desc << "\n";
        return 1;
    }
    
    vec3 pos, lookat;
    float aperture, dist_to_focus;
    
    unique_ptr<scene_manager> scene;
#ifndef RANDOM_SCENE
    pos = vec3(0, 2, 7);
    lookat = vec3::zero;
    aperture = 0;
    dist_to_focus = 1;
    
    scene = simple_scene(use_quadtree);
#else
    pos = vec3(13, 2, 3);
    lookat = vec3::zero;
    aperture = 0.1f;
    dist_to_focus = 10.0f;
    
    scene = random_scene(use_quadtree);
#endif

    scene->build_scene();
    
    camera cam(pos, lookat, vec3::up,
               20.0f, (float)img.width / img.height,
               aperture, dist_to_focus,
               0.0f, 1.0f);
    
    using namespace chrono;
    auto start = high_resolution_clock::now();

    render(*scene, cam, img, opts);

    auto end = high_resolution_clock::now();
    cout << "duration: " << duration_cast<milliseconds>(end - start).count() << "\n";
}
