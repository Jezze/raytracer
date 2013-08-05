#include <stdlib.h>
#include <math.h>
#include "vector3.h"
#include "color.h"
#include "material.h"

void material_checkers(struct material *material, struct vector3 *position)
{

    int square = (int)floor(position->x) + (int)floor(position->z);

    if ((square % 2) == 0)
    {

        material->diffuse.r = 0.2;
        material->diffuse.g = 0.2;
        material->diffuse.b = 0.2;

    }

    else
    {

        material->diffuse.r = 0.8;
        material->diffuse.g = 0.8;
        material->diffuse.b = 0.8;

    }

}

