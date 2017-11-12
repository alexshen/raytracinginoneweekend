#include "hitable_list.h"

#include <numeric>
#include <utility>
using namespace std;

bool hitable_list::hit(const ray& r, float tmin, float tmax, hit_record& rec) const
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

void hitable_list::add(hitable_ptr obj)
{
	auto p = obj.get();
    m_objects.push_back(move(obj));

	if (m_objects.size() == 1) {
		m_aabb = p->get_aabb();
	} else {
		m_aabb.expand(p->get_aabb());
	}
}
