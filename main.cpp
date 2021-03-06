#define MT

#include "block_texture.h"
#include "bvh_object.h"
#include "camera.h"
#include "constant_medium.h"
#include "constant_texture.h"
#include "checker_texture.h"
#include "dielectric.h"
#include "lambertian.h"
#include "material.h"
#include "metal.h"
#include "ray.h"
#include "scene_manager.h"
#include "bvh_manager.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "utils.h"
#include "vec.h"
#include "hitable_list.h"
#include "perlin.h"
#include "noise_texture.h"
#include "image_texture.h"
#include "diffuse_light.h"
#include "aa_rect.h"
#include "flip_normal.h"
#include "box.h"
#include "translation.h"
#include "rotation_y.h"

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

vec3 color(const ray3& r, scene_manager& scene, object_list& hitables, int depth = 0)
{
    hitables.clear();
    scene.root().raycast(r, hitables);

    hit_record rec;
    
    // 0.001 to avoid shadow acne
    if (check_hit(hitables, r, 0.001f, numeric_limits<float>::max(), rec)) {
        vec3 attenuation;
        ray3 scattered;
        vec3 emitted = rec.mat->emitted(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.mat->scatter(r, rec, attenuation, scattered)) {
            return emitted + attenuation * color(scattered, scene, hitables, depth + 1);
        } else {
            return emitted;
        }
    }

#if 0
    auto dir = normalize(r.dir);
    float t = 0.5f * (dir.y() + 1.0f);
    return lerp(vec3(1.0f, 1.0f, 1.0f), vec3(0.5f, 0.7f, 1.0f), t);
#else
    return vec3::zero;
#endif
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

inline unique_ptr<hitable> lambertian_sphere(const vec3& center, float r, material_ptr mat)
{
    return make_unique<sphere>(center, r, move(mat));
}

inline material_ptr lambertian_color(const vec3& albedo)
{
    return make_shared<lambertian>(texture_constant(albedo));
}

inline unique_ptr<hitable> lambertian_moving_sphere(const vec3& center0, const vec3& center1,
                                                    float time0, float time1,
                                                    float radius, const vec3& albedo)
{
    return make_unique<moving_sphere>(center0, center1,
                                      time0, time1,
                                      radius,
                                      lambertian_color(albedo));
}

inline unique_ptr<hitable> metal_sphere(const vec3& center, float r, const vec3& albedo, float fuzz = 0.0f)
{
    return make_unique<sphere>(center, r, make_shared<metal>(albedo, fuzz));
}

inline unique_ptr<hitable> dielectric_sphere(const vec3& center, float r, float index)
{
    return make_unique<sphere>(center, r, make_shared<dielectric>(index));
}

inline material_ptr light_source(const vec3& color)
{
    return make_shared<diffuse_light>(make_shared<constant_texture>(color));
}

inline unique_ptr<hitable> translate(unique_ptr<hitable> obj, const vec3& offset)
{
    return make_unique<translation>(move(obj), offset);
}

inline unique_ptr<hitable> rotate_y(unique_ptr<hitable> obj, float angle)
{
    return make_unique<rotation_y>(move(obj), angle);
}

inline unique_ptr<constant_medium> constant_color_medium(unique_ptr<hitable> boundary, float density, const vec3& albedo)
{
    return make_unique<constant_medium>(move(boundary), density, texture_constant(albedo));
}

struct options
{
    string output;
    int num_threads;
    bool report_progress;
};

unique_ptr<scene_manager> oneweekend_final_scene()
{
    auto scene = make_unique<bvh_manager>();
    std::vector<vec3> colors{
        vec3::one * 0.9f, vec3::one * 0.6f,
        vec3::one * 0.5f, vec3::one * 0.2f
    };
    auto block_tex = make_shared<block_texture>(colors, 2);
    auto checker_tex = make_shared<checker_texture>(texture_constant(vec3(0.2f, 0.3f, 0.1f)),
                                                    texture_constant(vec3::one * 0.9f));
    //scene->add(lambertian_sphere(vec3(0, -1000, 0), 1000, move(checker_tex)));
    scene->add(lambertian_sphere(vec3(0, -1000, 0), 1000, move(block_tex)));

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

unique_ptr<scene_manager> simple_scene()
{
    auto scene = make_unique<bvh_manager>();

    scene->add(lambertian_sphere(vec3(0, 0, -1), 0.5f, vec3(0.1f, 0.2f, 0.5f)));
    scene->add(lambertian_sphere(vec3(0, -100.5, -1), 100, vec3(0.8f, 0.8f, 0)));
    scene->add(metal_sphere(vec3(1, 0, -1), 0.5f, vec3(0.8f, 0.6f, 0.2f)));
    scene->add(dielectric_sphere(vec3(-1, 0, -1), 0.5f, 1.5f));
    scene->add(dielectric_sphere(vec3(-1, 0, -1), -0.45f, 1.5f));
    
    return scene;
}

unique_ptr<scene_manager> two_perlin_spheres()
{
    //auto tex = make_shared<noise_texture>(4);
    auto tex = image_texture::load("earthmap.jpg");
    auto scene = make_unique<bvh_manager>();
    scene->add(lambertian_sphere(vec3(0, -1000, 0), 1000, tex));
    scene->add(lambertian_sphere(vec3(0, 2, 0), 2, tex));
    return move(scene);
}

unique_ptr<scene_manager> simple_light()
{
    auto tex = make_shared<noise_texture>(4.0f);
    auto scene = make_unique<bvh_manager>();
    scene->add(lambertian_sphere(vec3(0, -1000, 0), 1000, tex));
    scene->add(lambertian_sphere(vec3(0, 2, 0), 2, tex));

    auto const_color_tex = make_shared<constant_texture>(vec3::one * 4.0f);
    auto light = make_shared<diffuse_light>(const_color_tex);
    scene->add(lambertian_sphere(vec3(0, 7, 0), 2, light));
    scene->add(aa_rect::xy(vec2(3, 1), vec2(5, 3), -2.0f, light));
    return move(scene);
}

unique_ptr<scene_manager> cornell_box()
{
    auto scene = make_unique<bvh_manager>();
    auto green = lambertian_color(vec3(0.12f, 0.45f, 0.15f));
    auto red = lambertian_color(vec3(0.65f, 0.05f, 0.05f));
    auto white = lambertian_color(vec3::one * 0.73f);
    auto light = light_source(vec3::one * 15.0f);

    scene->add<flip_normal>(aa_rect::yz(vec2::zero, vec2::one * 555.0f, 555.0f, green));
    scene->add(aa_rect::yz(vec2::zero, vec2::one * 555.0f, 0, red));
    scene->add(aa_rect::xz(vec2(213, 227), vec2(343, 332), 554.0f, light));

    scene->add(aa_rect::xz(vec2::zero, vec2::one * 555.0f, 0, white));
    scene->add<flip_normal>(aa_rect::xz(vec2::zero, vec2::one * 555.0f, 555.0f, white));

    scene->add<flip_normal>(aa_rect::xy(vec2::zero, vec2::one * 555.0f, 555.0f, white));

    auto box0 = make_unique<box>(vec3::zero, vec3::one * 165.0f, white);
    auto box1 = make_unique<box>(vec3::zero, vec3(165.0f, 330.0f, 165.0f), white);
    scene->add(translate(rotate_y(move(box0), -18.0f), vec3(130.0f, 0.0f, 65.0f)));
    scene->add(translate(rotate_y(move(box1), 15.0f), vec3(265.0f, 0.0f, 295.0f)));
    return scene;
}

unique_ptr<scene_manager> cornell_smoke()
{
    auto scene = make_unique<bvh_manager>();
    auto green = lambertian_color(vec3(0.12f, 0.45f, 0.15f));
    auto red = lambertian_color(vec3(0.65f, 0.05f, 0.05f));
    auto white = lambertian_color(vec3::one * 0.73f);
    auto light = light_source(vec3::one * 7.0f);

    scene->add<flip_normal>(aa_rect::yz(vec2::zero, vec2::one * 555.0f, 555.0f, green));
    scene->add(aa_rect::yz(vec2::zero, vec2::one * 555.0f, 0, red));

    scene->add(aa_rect::xz(vec2(113, 127), vec2(443, 432), 554.0f, light));
    scene->add<flip_normal>(aa_rect::xz(vec2::zero, vec2::one * 555.0f, 555.0f, white));
    scene->add(aa_rect::xz(vec2::zero, vec2::one * 555.0f, 0, white));

    scene->add<flip_normal>(aa_rect::xy(vec2::zero, vec2::one * 555.0f, 555.0f, white));

    auto box0 = make_unique<box>(vec3::zero, vec3::one * 165.0f, white);
    auto box1 = make_unique<box>(vec3::zero, vec3(165.0f, 330.0f, 165.0f), white);
    auto smoke0 = translate(rotate_y(move(box0), -18.0f), vec3(130.0f, 0.0f, 65.0f));
    auto smoke1 = translate(rotate_y(move(box1), 15.0f), vec3(265.0f, 0.0f, 295.0f));
    scene->add(constant_color_medium(move(smoke0), 0.01f, vec3::one));
    scene->add(constant_color_medium(move(smoke1), 0.01f, vec3::zero));
    return scene;
}

unique_ptr<scene_manager> nextweek_final_scene()
{
	auto scene = make_unique<bvh_manager>();
	auto white = lambertian_color(vec3::one * 0.73f);
	auto ground = lambertian_color(vec3(0.48f, 0.83f, 0.53f));
	constexpr int nb = 20;
	for (int i = 0; i < nb; ++i) {
		for (int j = 0; j < nb; ++j) {
			constexpr int w = 100;
			float x0 = -1000.0f + i * w;
			float z0 = -1000.0f + j * w;
			float y0 = 0;
			float x1 = x0 + w;
			float y1 = 100 * (random_unit() + 0.01f);
			float z1 = z0 + w;
			scene->add<box>(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
		}
	}

	auto light = light_source(vec3::one * 7.0f);
	scene->add(aa_rect::xz(vec2(123.0f, 147.0f), vec2(423.0f, 412.0f), 554.0f, light));

	vec3 center(400.0f, 400.0f, 200.0f);
	scene->add(lambertian_moving_sphere(center, center + vec3::right * 30.0f, 0.0f, 1.0f, 50.0f, vec3(0.7f, 0.3f, 0.1f)));
	scene->add(dielectric_sphere(vec3(260.0f, 150.0f, 45.0f), 50.0f, 1.5f));
	scene->add(metal_sphere(vec3(0.0f, 150.0f, 145.0f), 50, vec3(0.8f, 0.8f, 0.9f), 10.0f));

	scene->add(dielectric_sphere(vec3(360.0f, 150.0f, 145.0f), 70.0f, 1.5f));
	scene->add(dielectric_sphere(vec3(360.0f, 150.0f, 145.0f), 70.0f, 1.5f));

	scene->add(constant_color_medium(make_unique<sphere>(vec3(360.0f, 150.0f, 145.0f), 70.0f, nullptr), 0.2f, vec3(0.2f, 0.4f, 0.9f)));
	scene->add(constant_color_medium(make_unique<sphere>(vec3::zero, 5000.0f, nullptr), 0.0001f, vec3::one));

	auto earth_tex = image_texture::load("earthmap.jpg");
	scene->add(lambertian_sphere(vec3(400.0f, 200.0f, 400.0f), 100.0f, earth_tex));

	auto noise_tex = make_shared<noise_texture>(0.1f);
	scene->add(lambertian_sphere(vec3(220.0f, 280.0f, 300.0f), 80.0f, noise_tex));

	vector<unique_ptr<hitable>> spheres;
	for (int i = 0; i < 1000; ++i) {
		spheres.push_back(lambertian_sphere(vec3(random_unit(), random_unit(), random_unit()) * 165.0f, 10.0f, white));
	}
	auto box = make_unique<bvh_object>(move(spheres));
	scene->add(translate(rotate_y(move(box), 15.0f), vec3(-100.0f, 270.0f, 395.0f)));

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
                auto r = cam.get_ray(u, v);
                c += color(r, scene, hitables);
            }
            c /= (float)img.samples;
            
            // gamma correction
            auto ir = min((int)(sqrtf(c[0]) * 255.99f), 255);
            auto ig = min((int)(sqrtf(c[1]) * 255.99f), 255);
            auto ib = min((int)(sqrtf(c[2]) * 255.99f), 255);
            
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

int main(int argc, char* argv[])
{
    perlin::init();
    
    image img;
    options opts;
    
    po::options_description desc;
    desc.add_options()
        ("help,h", "help message")
        (",j", po::value<int>(&opts.num_threads)->default_value(thread::hardware_concurrency()), "threads for rendering")
        ("width", po::value<int>(&img.width)->default_value(300), "image width")
        ("height", po::value<int>(&img.height)->default_value(200), "image height")
        ("sample", po::value<int>(&img.samples)->default_value(100), "samples per pixel")
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
#if 0
    pos = vec3(0, 2, 7);
    lookat = vec3::zero;
    aperture = 0;
    dist_to_focus = 1;
    
    scene = simple_scene();
#else
    pos = vec3(478, 278, -600);
    lookat = vec3(278, 278, 0);
    aperture = 0.0f;
    dist_to_focus = 10.0f;
    
    //scene = oneweekend_final_scene();
    scene = nextweek_final_scene();
#endif

    scene->build_scene();
    
    camera cam(pos, lookat, vec3::up,
               40.0f, (float)img.width / img.height,
               aperture, dist_to_focus,
               0.0f, 1.0f);
    
    using namespace chrono;
    auto start = high_resolution_clock::now();

    render(*scene, cam, img, opts);

    auto end = high_resolution_clock::now();
    cout << "duration: " << duration_cast<milliseconds>(end - start).count() << "\n";
}
