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

struct light
{

    struct vector3 position;
    struct color color;

};

struct object
{

    struct color color;
    struct vector3 (*get_normal_at)(struct object *self, struct vector3 *v);
    double (*find_intersection)(struct object *self, struct ray *r);

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

void render(unsigned int w, unsigned int h, struct bmp_color *data, struct camera *camera, unsigned int count, struct object *objects[])
{

    struct color black = {0.0, 0.0, 0.0, 0.0};
    double *intersections = malloc(sizeof (double) * count);
    double aspectratio = (double)w / (double)h;
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

            struct vector3 cam_ray_origin = camera->position;
            struct vector3 cam_ray_direction;
            struct vector3 a = vector3_multiply(&camera->right, xamnt - 0.5);
            struct vector3 b = vector3_multiply(&camera->down, yamnt - 0.5);
            struct vector3 c = vector3_add(&a, &b);

            c = vector3_add(&camera->direction, &c);
            cam_ray_direction = vector3_normalize(&c);

            struct ray cam_ray = {cam_ray_origin, cam_ray_direction};

            for (i = 0; i < count; i++)
                intersections[i] = objects[i]->find_intersection(objects[i], &cam_ray);

            int index = find_closest(intersections, count);
            struct color *cc = (index == - 1) ? &black : &objects[index]->color;

            current->r = cc->r;
            current->g = cc->g;
            current->b = cc->b;

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
    struct vector3 x = {1.0, 0.0, 0.0};
    struct vector3 y = {0.0, 1.0, 0.0};
    struct vector3 z = {0.0, 0.0, 1.0};
    struct vector3 lookat = {0.0, 0.0, 0.0};
    struct vector3 campos = {3.0, 1.5, -4.0};
    struct vector3 diff_btw = vector3_subtract(&campos, &lookat);

    struct vector3 camdir = vector3_negative(&diff_btw);

    camdir = vector3_normalize(&camdir);

    struct vector3 camright = vector3_crossproduct(&y, &camdir);

    camright = vector3_normalize(&camright);

    struct vector3 camdown = vector3_crossproduct(&camright, &camdir);
    struct camera scene_cam = {campos, camdir, camright, camdown};

    struct color white_light = {1.0, 1.0, 1.0, 0.0};
    struct color pretty_green = {0.5, 1.0, 0.5, 0.3};
    struct color gray = {0.5, 0.5, 0.5, 0.0};
    struct color black = {0.0, 0.0, 0.0, 0.0};
    struct color maroon = {0.5, 0.25, 0.25, 0.0};

    struct vector3 light_position = {-7.0, 10.0, -10.0};
    struct light scene_light = {light_position, white_light};

    struct sphere scene_sphere = {{pretty_green, sphere_get_normal_at, sphere_find_intersection}, origin, 1.0};
    struct plane scene_plane = {{maroon, plane_get_normal_at, plane_find_intersection}, y, -1.0};
    struct object *scene_objects[] = {&scene_sphere.base, &scene_plane.base};

    render(w, h, data, &scene_cam, 2, scene_objects);
    save("scene.bmp", w, h, dpi, w * h, data);
    free(data);

    return 0;

}

