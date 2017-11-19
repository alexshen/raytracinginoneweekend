#ifndef MAT_H
#define MAT_H

#include "vec.h"
#include <kismet/math/matrix.h>

template<typename T, std::size_t N>
using matrix = kismet::math::matrix<T, N, N>;
using mat2 = kismet::math::matrix22f;
using mat3 = kismet::math::matrix33f;

mat2 mat2_rotate_y(float angle);
mat3 mat3_rotate_y(float angle);

template<typename T>
struct mat_type;

template<>
struct mat_type<vec2>
{
    using type = mat2;
};

template<>
struct mat_type<vec3>
{
    using type = mat3;
};

template<typename T>
using mat_t = matrix<typename T::value_type, std::tuple_size_v<T>>;

template<typename T>
mat_t<T> mat_rotate_y(float angle);

template<>
inline mat2 mat_rotate_y<vec2>(float angle)
{
    return mat2_rotate_y(angle);
}

template<>
inline mat3 mat_rotate_y<vec3>(float angle)
{
    return mat3_rotate_y(angle);
}

#endif /* MAT_H */
