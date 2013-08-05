#include <stdlib.h>
#include "vector3.h"

struct camera
{

    struct vector3 position;
    struct vector3 direction;
    struct vector3 right;
    struct vector3 down;

};

void camera_init(struct camera *camera, struct vector3 *origin, struct vector3 *originy)
{

    struct vector3 position = {3.0, 1.5, -4.0};
    struct vector3 direction = position;
    struct vector3 right;
    struct vector3 down;

    vector3_subtract(&direction, origin);
    vector3_negative(&direction);
    vector3_normalize(&direction);

    right = vector3_crossproduct(originy, &direction);

    vector3_normalize(&right);

    down = vector3_crossproduct(&right, &direction);

    camera->position = position;
    camera->direction = direction;
    camera->right = right;
    camera->down = down;

}

