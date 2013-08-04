#include <stdlib.h>
#include <math.h>
#include "vector3.h"

double vector3_magnitude(struct vector3 *v)
{

    return sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z));

}

double vector3_dotproduct(struct vector3 *v1, struct vector3 *v2)
{

    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;

}

struct vector3 vector3_normalize(struct vector3 *v)
{

    double magnitude = vector3_magnitude(v);
    struct vector3 t = {v->x / magnitude, v->y / magnitude, v->z / magnitude};

    return t;

}

struct vector3 vector3_negative(struct vector3 *v)
{

    struct vector3 t = {-v->x, -v->y, -v->z};

    return t;

}

struct vector3 vector3_crossproduct(struct vector3 *v1, struct vector3 *v2)
{

    struct vector3 t = {v1->y * v2->z - v1->z * v2->y, v1->z * v2->x - v1->x * v2->z, v1->x * v2->y - v1->y * v2->x};

    return t;

}

struct vector3 vector3_add(struct vector3 *v1, struct vector3 *v2)
{

    struct vector3 t = {v1->x + v2->x, v1->y + v2->y, v1->z + v2->z};

    return t;

}

struct vector3 vector3_subtract(struct vector3 *v1, struct vector3 *v2)
{

    struct vector3 t = {v1->x - v2->x, v1->y - v2->y, v1->z - v2->z};

    return t;

}

struct vector3 vector3_multiply(struct vector3 *v1, double scalar)
{

    struct vector3 t = {v1->x * scalar, v1->y * scalar, v1->z * scalar};

    return t;

}

