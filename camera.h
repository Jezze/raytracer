struct camera
{

    struct vector3 position;
    struct vector3 direction;
    struct vector3 right;
    struct vector3 down;

};

void camera_init(struct camera *camera, struct vector3 *origin, struct vector3 *originy);
