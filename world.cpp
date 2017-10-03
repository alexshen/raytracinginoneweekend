#include "world.h"

#include <numeric>
#include <utility>
using namespace std;

bool world::hit(const ray& r, float tmin, float tmax, hit_record& rec) const
{
    bool hit = false;
    rec.t = tmax;
    hit_record tmp;
    for (const auto& o: m_objects) {
        if (o->hit(r, tmin, rec.t, tmp)) {
            rec = tmp;
            hit = true;
        }
    }
    return hit;
}

void world::add(hitable_ptr p)
{
    m_objects.push_back(move(p));
}
