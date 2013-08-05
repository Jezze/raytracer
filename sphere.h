struct sphere
{

    struct entity base;
    struct vector3 center;
    double radius;

};

struct vector3 sphere_find_normal(struct entity *self, struct vector3 *v);
double sphere_find_intersection(struct entity *self, struct ray *r);
