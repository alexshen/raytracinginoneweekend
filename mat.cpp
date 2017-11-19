#include "mat.h"
#include "utils.h"

mat2 mat2_rotate_y(float angle)
{
	float cosine;
	float sine = sincos(angle, cosine);

    mat2 m;

    m[0][0] = cosine;
    m[0][1] = -sine;

    m[1][0] = sine;
    m[1][1] = cosine;

    return m;
}

mat3 mat3_rotate_y(float angle)
{
	float cosine;
	float sine = sincos(angle, cosine);

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
