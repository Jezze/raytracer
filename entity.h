struct entity
{

    struct material material;
    struct vector3 (*find_normal)(struct entity *self, struct vector3 *v);
    double (*find_intersection)(struct entity *self, struct ray *r);

};
