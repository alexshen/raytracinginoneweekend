#include "mat.h"
#include "utils.h"

#include <cmath>

mat2 mat2_rotate(float angle)
{
    auto [sine, cosine] = sincos(angle);

    mat2 m;

    m[0][0] = cosine;
    m[0][1] = -sine;

    m[1][0] = sine;
    m[1][1] = cosine;

    return m;
}

mat3 mat3_rotate(float angle)
{
    auto [sine, cosine] = sincos(angle);

    mat3 m;

    m[0][0] = cosine;
    m[0][1] = 0.0f;
    m[0][2] = sine;

    m[1][0] = 0.0f;
    m[1][1] = 1.0f;
    m[1][2] = 0.0f;

    m[2][0] = -sine;
    m[2][1] = 0.0f;
    m[2][2] = cosine;

    return m;
}
