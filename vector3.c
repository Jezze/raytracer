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

void vector3_normalize(struct vector3 *v)
{

    double magnitude = vector3_magnitude(v);

    v->x = v->x / magnitude;
    v->y = v->y / magnitude;
    v->z = v->z / magnitude;

}

void vector3_negative(struct vector3 *v)
{

    v->x = -v->x;
    v->y = -v->y;
    v->z = -v->z;

}

struct vector3 vector3_crossproduct(struct vector3 *v1, struct vector3 *v2)
{

    struct vector3 t;
    
    t.x = v1->y * v2->z - v1->z * v2->y;
    t.y = v1->z * v2->x - v1->x * v2->z;
    t.z = v1->x * v2->y - v1->y * v2->x;

    return t;

}

void vector3_add(struct vector3 *v1, struct vector3 *v2)
{

    v1->x += v2->x;
    v1->y += v2->y;
    v1->z += v2->z;

}

void vector3_subtract(struct vector3 *v1, struct vector3 *v2)
{

    v1->x -= v2->x;
    v1->y -= v2->y;
    v1->z -= v2->z;

}

void vector3_scalar(struct vector3 *v, double scalar)
{

    v->x *= scalar;
    v->y *= scalar;
    v->z *= scalar;

}

