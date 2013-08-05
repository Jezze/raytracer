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
    double b = (2 * (r->origin.x - s->center.x) * r->direction.x) + (2 * (r->origin.y - s->center.y) * r->direction.y) + (2 * (r->origin.z - s->center.z) * r->direction.z);
    double c = pow(r->origin.x - s->center.x, 2) + pow(r->origin.y - s->center.y, 2) + pow(r->origin.z - s->center.z, 2) - (s->radius * s->radius);
    double discriminant = b * b - 4 * c;
    double root;

    if (discriminant <= 0)
        return -1;

    root = ((-1 * b - sqrt(discriminant)) / 2) - 0.000001;

    if (root > 0)
        return root;

    return ((sqrt(discriminant) - b) / 2) - 0.000001;

}

