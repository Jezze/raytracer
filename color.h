struct color
{

    double r;
    double g;
    double b;
    double special;

};

double color_brightness(struct color *color);
struct color color_scalar(struct color *color, double scalar);
struct color color_add(struct color *c1, struct color *c2);
struct color color_multiply(struct color *c1, struct color *c2);
struct color color_average(struct color *c1, struct color *c2);
void color_clip(struct color *color);
