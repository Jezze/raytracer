#include <stdlib.h>
#include "color.h"

float color_brightness(struct color *color)
{

    return (color->r + color->g + color->b) / 3.0;

}

void color_scalar(struct color *color, float scalar)
{

    color->r *= scalar;
    color->g *= scalar;
    color->b *= scalar;

}

void color_add(struct color *c1, struct color *c2)
{

    c1->r += c2->r;
    c1->g += c2->g;
    c1->b += c2->b;

}

void color_multiply(struct color *c1, struct color *c2)
{

    c1->r *= c2->r;
    c1->g *= c2->g;
    c1->b *= c2->b;

}

void color_average(struct color *c1, struct color *c2)
{

    c1->r = (c1->r + c2->r) / 2.0;
    c1->g = (c1->g + c2->g) / 2.0;
    c1->b = (c1->b + c2->b) / 2.0;

}

void color_clip(struct color *color)
{

    float all = color->r + color->g + color->b;
    float extra = all - 3.0;

    if (extra > 0.0)
    {

        color->r = color->r + extra * (color->r / all);
        color->g = color->g + extra * (color->g / all);
        color->b = color->b + extra * (color->b / all);

    }

    if (color->r > 1.0)
        color->r = 1.0;

    if (color->g > 1.0)
        color->g = 1.0;

    if (color->b > 1.0)
        color->b = 1.0;

    if (color->r < 0.0)
        color->r = 0.0;

    if (color->g < 0.0)
        color->g = 0.0;

    if (color->b < 0.0)
        color->b = 0.0;

}

