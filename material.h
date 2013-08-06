enum material_type
{

    MATERIAL_TYPE_NORMAL                = 0,
    MATERIAL_TYPE_GOURAD                = 1,
    MATERIAL_TYPE_NOICE                 = 2,
    MATERIAL_TYPE_CHECKERS              = 3

};

struct material
{

    enum material_type type;
    struct color diffuse;
    double reflection;
    double refraction;

};

void material_checkers(struct material *material, struct vector3 *position);
