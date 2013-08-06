#include <stdlib.h>
#include <math.h>
#include "vector3.h"
#include "color.h"
#include "material.h"
#include "ray.h"
#include "entity.h"
#include "sphere.h"

struct vector3 sphere_find_normal(struct entity *self, struct vector3 *v)
{

    struct sphere *s = (struct sphere *)self;
    struct vector3 a = s->center;
    struct vector3 b = *v;

    vector3_negative(&a);
    vector3_add(&b, &a);
    vector3_normalize(&b);

    return b;

}

double sphere_find_intersection(struct entity *self, struct ray *r)
{

    struct sphere *s = (struct sphere *)self;
    struct vector3 v = s->center;
    double b, d, t;

    vector3_subtract(&v, &r->origin);

    b = vector3_dotproduct(&r->direction, &v);
    d = b * b - vector3_dotproduct(&v, &v) + s->radius * s->radius;

    if (d <= 0)
        return -1;

    t = b - sqrt(d);

    if (t > 0.000001)
        return t;

    t = b + sqrt(d);

    if (t > 0.000001)
        return t;

    return -1;

}

