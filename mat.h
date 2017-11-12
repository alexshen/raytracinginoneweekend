#ifndef MAT_H
#define MAT_H

#include <kismet/math/matrix.h>

using mat2 = kismet::math::matrix22f;
using mat3 = kismet::math::matrix33f;

mat2 mat2_rotate(float angle);
mat3 mat3_rotate_y(float angle);

#endif /* MAT_H */
