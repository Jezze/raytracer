#include <stdlib.h>
#include "color.h"

double color_brightness(struct color *color)
{

    return (color->r + color->g + color->b) / 3;

}

struct color color_scalar(struct color *color, double scalar)
{

    struct color c = {color->r * scalar, color->g * scalar, color->b * scalar};

    return c;

}

struct color color_add(struct color *c1, struct color *c2)
{

    struct color c = {c1->r + c2->r, c1->g + c2->g, c1->b + c2->b};

    return c;

}

struct color color_multiply(struct color *c1, struct color *c2)
{

    struct color c = {c1->r * c2->r, c1->g * c2->g, c1->b * c2->b};

    return c;

}

struct color color_average(struct color *c1, struct color *c2)
{

    struct color c = {(c1->r + c2->r) / 2, (c1->g + c2->g) / 2, (c1->b + c2->b) / 2};

    return c;

}

void color_clip(struct color *color)
{

    double alllight = color->r + color->g + color->b;
    double excesslight = alllight - 3;
    struct color c;

    if (excesslight > 0)
    {

        color->r = color->r + excesslight * (color->r / alllight);
        color->g = color->g + excesslight * (color->g / alllight);
        color->b = color->b + excesslight * (color->b / alllight);

    }

    if (color->r > 1)
        color->r = 1;

    if (color->g > 1)
        color->g = 1;

    if (color->b > 1)
        color->b = 1;

    if (color->r < 0)
        color->r = 0;

    if (color->g < 0)
        color->g = 0;

    if (color->b < 0)
        color->b = 0;

}

