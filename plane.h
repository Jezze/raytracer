struct plane
{

    struct entity base;
    struct vector3 normal;
    double distance;

};

struct vector3 plane_find_normal(struct entity *self, struct vector3 *v);
double plane_find_intersection(struct entity *self, struct ray *r);
