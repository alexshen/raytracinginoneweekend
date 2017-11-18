#include "hitable.h"

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
