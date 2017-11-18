#include "constant_medium.h"
#include "utils.h"
#include <cfloat>
#include <algorithm>
#include <cmath>
using namespace std;

bool constant_medium::hit(const ray& r, float tmin, float tmax, hit_record& rec) const
{
    hit_record rec1, rec2;
    if (m_boundary->hit(r, -FLT_MAX, FLT_MAX, rec1) &&
        m_boundary->hit(r, rec1.t + 0.0001f, FLT_MAX, rec2)) {
        rec1.t = max(rec1.t, tmin);
        rec2.t = min(rec2.t, tmax);
        if (rec1.t >= rec2.t) {
            return false;
        }

        rec1.t = max(0.0f, rec1.t);
        float dmag = r.dir.mag(); 
        float no_hit_dist = (rec2.t - rec1.t) * dmag;
        float dist = -1.0f / m_density * log(random_unit());
        if (dist < no_hit_dist) {
            rec.t = rec1.t + dist / dmag;
            rec.p = r.point_at(rec.t);
            rec.mat = m_phase_function.get();
            rec.normal = vec3::forward;
            rec.u = rec.v = 0;
            return true;
        }
    }
    return false;
}
