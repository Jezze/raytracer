struct entity
{

    struct color color;
    double reflection;
    unsigned int texture;
    struct vector3 (*find_normal)(struct entity *self, struct vector3 *v);
    double (*find_intersection)(struct entity *self, struct ray *r);

};
