#include <stdlib.h>
#include <math.h>
#include "vector3.h"
#include "color.h"
#include "ray.h"
#include "entity.h"
#include "plane.h"

struct vector3 plane_find_normal(struct entity *self, struct vector3 *v)
{

    struct plane *p = (struct plane *)self;

    return p->normal;

}

double plane_find_intersection(struct entity *self, struct ray *r)
{

    struct plane *p = (struct plane *)self;
    double a = vector3_dotproduct(&r->direction, &p->normal);
    struct vector3 v = p->normal;

    if (!a)
        return -1;

    vector3_scalar(&v, p->distance);
    vector3_negative(&v);
    vector3_add(&v, &r->origin);

    return -1 * vector3_dotproduct(&p->normal, &v) / a;

}

