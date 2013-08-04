#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

struct bmp_color
{

    double r;
    double g;
    double b;

};

struct vector3
{

    double x;
    double y;
    double z;

};

struct ray
{

    struct vector3 origin;
    struct vector3 direction;

};

struct camera
{

    struct vector3 position;
    struct vector3 direction;
    struct vector3 right;
    struct vector3 down;

};

struct color
{

    double r;
    double g;
    double b;
    double special;

};

struct source
{

    struct color color;
    struct vector3 (*get_position)(struct source *self);

};

struct object
{

    struct color color;
    struct vector3 (*get_normal_at)(struct object *self, struct vector3 *v);
    double (*find_intersection)(struct object *self, struct ray *r);

};

struct light
{

    struct source base;
    struct vector3 position;

};

struct plane
{

    struct object base;
    struct vector3 normal;
    double distance;

};

struct sphere
{

    struct object base;
    struct vector3 center;
    double radius;

};

static struct vector3 light_get_position(struct source *self)
{

    struct light *l = (struct light *)self;

    return l->position;

}

static double color_brightness(struct color *color)
{

    return (color->r + color->g + color->b) / 3;

}

static struct color color_scalar(struct color *color, double scalar)
{

    struct color c = {color->r * scalar, color->g * scalar, color->b * scalar, color->special};

    return c;

}

static struct color color_add(struct color *c1, struct color *c2)
{

    struct color c = {c1->r + c2->r, c1->g + c2->g, c1->b + c2->b, c1->special};

    return c;

}

static struct color color_multiply(struct color *c1, struct color *c2)
{

    struct color c = {c1->r * c2->r, c1->g * c2->g, c1->b * c2->b, c1->special};

    return c;

}

static struct color color_average(struct color *c1, struct color *c2)
{

    struct color c = {(c1->r + c2->r) / 2, (c1->g + c2->g) / 2, (c1->b + c2->b) / 2, c1->special};

    return c;

}

static void color_clip(struct color *color)
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

static double vector3_magnitude(struct vector3 *v)
{

    return sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z));

}

static double vector3_dotproduct(struct vector3 *v1, struct vector3 *v2)
{

    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;

}

static struct vector3 vector3_normalize(struct vector3 *v)
{

    double magnitude = vector3_magnitude(v);
    struct vector3 t = {v->x / magnitude, v->y / magnitude, v->z / magnitude};

    return t;

}

static struct vector3 vector3_negative(struct vector3 *v)
{

    struct vector3 t = {-v->x, -v->y, -v->z};

    return t;

}

static struct vector3 vector3_crossproduct(struct vector3 *v1, struct vector3 *v2)
{

    struct vector3 t = {v1->y * v2->z - v1->z * v2->y, v1->z * v2->x - v1->x * v2->z, v1->x * v2->y - v1->y * v2->x};

    return t;

}

static struct vector3 vector3_add(struct vector3 *v1, struct vector3 *v2)
{

    struct vector3 t = {v1->x + v2->x, v1->y + v2->y, v1->z + v2->z};

    return t;

}

static struct vector3 vector3_subtract(struct vector3 *v1, struct vector3 *v2)
{

    struct vector3 t = {v1->x - v2->x, v1->y - v2->y, v1->z - v2->z};

    return t;

}

static struct vector3 vector3_multiply(struct vector3 *v1, double scalar)
{

    struct vector3 t = {v1->x * scalar, v1->y * scalar, v1->z * scalar};

    return t;

}

static struct vector3 plane_get_normal_at(struct object *self, struct vector3 *v)
{

    struct plane *p = (struct plane *)self;

    return p->normal;

}

static double plane_find_intersection(struct object *self, struct ray *r)
{

    struct plane *p = (struct plane *)self;
    double a = vector3_dotproduct(&r->direction, &p->normal);
    struct vector3 v;

    if (!a)
        return -1;

    v = vector3_multiply(&p->normal, p->distance);
    v = vector3_negative(&v);
    v = vector3_add(&r->origin, &v);

    return -1 * vector3_dotproduct(&p->normal, &v) / a;

}

static struct vector3 sphere_get_normal_at(struct object *self, struct vector3 *v)
{

    struct sphere *s = (struct sphere *)self;
    struct vector3 a = vector3_negative(&s->center);
    struct vector3 b = vector3_add(v, &a);

    return vector3_normalize(&b);

}

static double sphere_find_intersection(struct object *self, struct ray *r)
{

    struct sphere *s = (struct sphere *)self;
    double b = (2 * (r->origin.x - s->center.x) * r->direction.x) + (2 * (r->origin.y - s->center.y) * r->direction.y) + (2 * (r->origin.z - s->center.z) * r->direction.z);
    double c = pow(r->origin.x - s->center.x, 2) + pow(r->origin.y - s->center.y, 2) + pow(r->origin.z - s->center.z, 2) - (s->radius * s->radius);
    double discriminant = b * b - 4 * c;
    double root;

    if (discriminant <= 0)
        return -1;

    root = ((-1 * b - sqrt(discriminant)) / 2) - 0.000001;

    if (root > 0)
        return root;

    return ((sqrt(discriminant) - b) / 2) - 0.000001;

}

void save(const char *name, int w, int h, int dpi, unsigned int count, struct bmp_color *data)
{

    FILE *f;
    unsigned int s = 4 * count;
    unsigned int filesize = 54 + s;
    unsigned int i;
    double factor = 39.375;
    int m = (int)factor;
    int ppm = dpi * m;
    unsigned char bmpfileheader[14] = {'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0};
    unsigned char bmpinfoheader[40] = {40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0};

    bmpfileheader[2] = (unsigned char)(filesize);
    bmpfileheader[3] = (unsigned char)(filesize >> 8);
    bmpfileheader[4] = (unsigned char)(filesize >> 16);
    bmpfileheader[5] = (unsigned char)(filesize >> 24);

    bmpinfoheader[4] = (unsigned char)(w);
    bmpinfoheader[5] = (unsigned char)(w >> 8);
    bmpinfoheader[6] = (unsigned char)(w >> 16);
    bmpinfoheader[7] = (unsigned char)(w >> 24);

    bmpinfoheader[8] = (unsigned char)(h);
    bmpinfoheader[9] = (unsigned char)(h >> 8);
    bmpinfoheader[10] = (unsigned char)(h >> 16);
    bmpinfoheader[11] = (unsigned char)(h >> 24);

    bmpinfoheader[21] = (unsigned char)(s);
    bmpinfoheader[22] = (unsigned char)(s >> 8);
    bmpinfoheader[23] = (unsigned char)(s >> 16);
    bmpinfoheader[24] = (unsigned char)(s >> 24);

    bmpinfoheader[25] = (unsigned char)(ppm);
    bmpinfoheader[26] = (unsigned char)(ppm >> 8);
    bmpinfoheader[27] = (unsigned char)(ppm >> 16);
    bmpinfoheader[28] = (unsigned char)(ppm >> 24);

    bmpinfoheader[29] = (unsigned char)(ppm);
    bmpinfoheader[30] = (unsigned char)(ppm >> 8);
    bmpinfoheader[31] = (unsigned char)(ppm >> 16);
    bmpinfoheader[32] = (unsigned char)(ppm >> 24);

    f = fopen(name, "wb");

    fwrite(bmpfileheader, 1, 14, f);
    fwrite(bmpinfoheader, 1, 40, f);

    for (i = 0; i < count; i++)
    {

        struct bmp_color rgb = data[i];
        double r = (data[i].r) * 255;
        double g = (data[i].g) * 255;
        double b = (data[i].b) * 255;
        unsigned char color[3] = {(int)floor(b), (int)floor(g), (int)floor(r)};

        fwrite(color, 1, 3, f);

    }

    fclose(f);

}

static int find_closest(double *intersection, unsigned int count)
{

    unsigned int index = -1;
    double max = -1;
    unsigned int i;

    if (!count)
       return -1;

    for (i = 0; i < count; i++)
    {

        if (intersection[i] > max)
            max = intersection[i];

    }

    if (max < 0)
        return -1;

    for (i = 0; i < count; i++)
    {

        if (intersection[i] < 0)
            continue;

        if (intersection[i] > max)
            continue;

        max = intersection[i];
        index = i;

    }

    return index;

}

static struct color get_color_at(struct vector3 *intersection_position, struct vector3 *intersecting_ray_direction, int index, int nobjects, struct object *objects[], int nlights, struct source *lights[], double accuracy, double ambientlight, double intersections[])
{

    struct color winning_object_color = objects[index]->color;
    struct vector3 winning_object_normal = objects[index]->get_normal_at(objects[index], intersection_position);
    unsigned int i;

    if (winning_object_color.special == 2)
    {

        int square = (int)floor(intersection_position->x) + (int)floor(intersection_position->z);

        if ((square % 2) == 0)
        {

            winning_object_color.r = 0.0;
            winning_object_color.g = 0.0;
            winning_object_color.b = 0.0;

        }

        else
        {

            winning_object_color.r = 1.0;
            winning_object_color.g = 1.0;
            winning_object_color.b = 1.0;

        }

    }

    struct color final_color = color_scalar(&winning_object_color, ambientlight);

    for (i = 0; i < nlights; i++)
    {

        struct vector3 position = lights[i]->get_position(lights[i]);
        struct vector3 a = vector3_negative(intersection_position);
        struct vector3 light_direction = vector3_add(&position, &a);
        unsigned int j;

        light_direction = vector3_normalize(&light_direction);

        double cosine_angle = vector3_dotproduct(&winning_object_normal, &light_direction);

        if (cosine_angle > 0)
        {

            unsigned int shadowed = 0;
            double magnitude = vector3_magnitude(&light_direction);
            struct vector3 f = vector3_add(&position, &light_direction);
            struct ray shadow_ray = {*intersection_position, vector3_normalize(&f)};

            for (j = 0; j < nobjects && shadowed == 0; j++)
                intersections[j] = objects[j]->find_intersection(objects[j], &shadow_ray);

            for (j = 0; j < nobjects; j++)
            {

                if (intersections[j] > accuracy)
                {

                    if (intersections[j] <= magnitude)
                        shadowed = 1;

                    break;

                }

            }

            if (shadowed == 0)
            {

                struct color wc = color_multiply(&winning_object_color, &lights[i]->color);

                wc = color_scalar(&wc, cosine_angle);

                final_color = color_add(&final_color, &wc);

                if (winning_object_color.special > 0 && winning_object_color.special <= 1)
                {

                    struct vector3 y = vector3_negative(intersecting_ray_direction);
                    double dot1 = vector3_dotproduct(&winning_object_normal, &y);
                    struct vector3 scalar1 = vector3_multiply(&winning_object_normal, dot1);
                    struct vector3 add1 = vector3_add(&scalar1, intersecting_ray_direction);
                    struct vector3 scalar2 = vector3_multiply(&add1, 2);
                    struct vector3 add2 = vector3_add(&y, &scalar2);
                    struct vector3 reflection_direction = vector3_normalize(&add2);
                    double specular = vector3_dotproduct(&reflection_direction, &light_direction);

                    if (specular > 0)
                    {

                        struct color shine = color_scalar(&lights[i]->color, pow(specular, 10) * winning_object_color.special);

                        final_color = color_add(&final_color, &shine);

                    }

                }

            }

        }

    }

    color_clip(&final_color);

    return final_color;

}

void render(unsigned int w, unsigned int h, struct bmp_color *data, struct camera *camera, unsigned int nobjects, struct object *objects[], unsigned int nlights, struct source *lights[])
{

    struct color black = {0.0, 0.0, 0.0, 0.0};
    double *intersections = malloc(sizeof (double) * nobjects);
    double *intersections2 = malloc(sizeof (double) * nobjects);
    double aspectratio = (double)w / (double)h;
    double ambientlight = 0.2;
    double accuracy = 0.000001;
    unsigned int x;
    unsigned int y;
    double xamnt, yamnt;
    unsigned int i;

    for (x = 0; x < w; x++)
    {

        for (y = 0; y < h; y++)
        {

            struct bmp_color *current = &data[y * w + x];

            if (w > h)
            {

                xamnt = ((x + 0.5) / w) * aspectratio - (((w - h) / (double)h) / 2);
                yamnt = ((h - y) + 0.5) / h;

            }

            else if (h > w)
            {

                xamnt = (x + 0.5) / w;
                yamnt = (((h - y) + 0.5) / h) / aspectratio - (((h - w) / (double)w) / 2);

            }

            else
            {

                xamnt = (x + 0.5) / w;
                yamnt = ((h - y) + 0.5) / h;

            }

            struct ray camray;
            struct vector3 a = vector3_multiply(&camera->right, xamnt - 0.5);
            struct vector3 b = vector3_multiply(&camera->down, yamnt - 0.5);
            struct vector3 c = vector3_add(&a, &b);

            c = vector3_add(&camera->direction, &c);

            camray.origin = camera->position;
            camray.direction = vector3_normalize(&c);

            for (i = 0; i < nobjects; i++)
                intersections[i] = objects[i]->find_intersection(objects[i], &camray);

            int index = find_closest(intersections, nobjects);

            if (index == -1)
            {

                current->r = black.r;
                current->g = black.g;
                current->b = black.b;

            }

            else
            {

                if (intersections[index] > accuracy)
                {

                    struct vector3 q = vector3_multiply(&camray.direction, intersections[index]);
                    struct vector3 intersection_position = vector3_add(&camray.origin, &q);
                    struct color intersection_color = get_color_at(&intersection_position, &camray.direction, index, nobjects, objects, nlights, lights, accuracy, ambientlight, intersections2);

                    current->r = intersection_color.r;
                    current->g = intersection_color.g;
                    current->b = intersection_color.b;

                }

            }

        }

    }

    free(intersections);

}

int main(int argc, char **argv)
{

    unsigned int w = 640;
    unsigned int h = 480;
    unsigned int dpi = 72;
    struct bmp_color *data = malloc(sizeof (struct bmp_color) * w * h);
    struct vector3 origin = {0.0, 0.0, 0.0};
    struct vector3 originx = {1.0, 0.0, 0.0};
    struct vector3 originy = {0.0, 1.0, 0.0};
    struct vector3 originz = {0.0, 0.0, 1.0};
    struct color white = {1.0, 1.0, 1.0, 0.0};
    struct color green = {0.5, 1.0, 0.5, 0.3};
    struct color gray = {0.5, 0.5, 0.5, 0.0};
    struct color black = {0.0, 0.0, 0.0, 0.0};
    struct color brown = {0.5, 0.25, 0.25, 2.0};
    struct vector3 lookat = origin;
    struct vector3 campos = {3.0, 1.5, -4.0};
    struct vector3 diff_btw = vector3_subtract(&campos, &lookat);

    struct vector3 camdir = vector3_negative(&diff_btw);

    camdir = vector3_normalize(&camdir);

    struct vector3 camright = vector3_crossproduct(&originy, &camdir);

    camright = vector3_normalize(&camright);

    struct vector3 camdown = vector3_crossproduct(&camright, &camdir);
    struct camera camera = {campos, camdir, camright, camdown};

    struct light light00 = {{white, light_get_position}, {-7.0, 10.0, -10.0}};
    struct sphere sphere00 = {{green, sphere_get_normal_at, sphere_find_intersection}, origin, 1.0};
    struct plane plane00 = {{brown, plane_get_normal_at, plane_find_intersection}, originy, -1.0};

    struct object *objects[] = {&sphere00.base, &plane00.base};
    struct source *lights[] = {&light00.base};

    render(w, h, data, &camera, 2, objects, 1, lights);
    save("scene.bmp", w, h, dpi, w * h, data);
    free(data);

    return 0;

}

