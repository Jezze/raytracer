#include <stdlib.h>
#include <math.h>
#include "vector3.h"
#include "color.h"
#include "ray.h"
#include "entity.h"
#include "texture.h"

void texture_checkerboard(struct entity *entity, struct vector3 *position)
{

    int square = (int)floor(position->x) + (int)floor(position->z);

    if ((square % 2) == 0)
    {

        entity->color.r = 0.2;
        entity->color.g = 0.2;
        entity->color.b = 0.2;

    }

    else
    {

        entity->color.r = 0.8;
        entity->color.g = 0.8;
        entity->color.b = 0.8;

    }

}

