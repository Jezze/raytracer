struct color
{

    double r;
    double g;
    double b;

};

double color_brightness(struct color *color);
void color_scalar(struct color *color, double scalar);
void color_add(struct color *c1, struct color *c2);
void color_multiply(struct color *c1, struct color *c2);
void color_average(struct color *c1, struct color *c2);
void color_clip(struct color *color);
