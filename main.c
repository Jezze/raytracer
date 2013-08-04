#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

struct rgbtype
{

    double r;
    double g;
    double b;

};

struct vector
{

    double x;
    double y;
    double z;

};

struct ray
{

    struct vector origin;
    struct vector direction;

};

struct camera
{

    struct vector position;
    struct vector direction;
    struct vector right;
    struct vector down;

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

    struct vector position;
    struct color color;

};

struct object
{

    struct color color;
    struct vector (*get_normal_at)(struct object *self, struct vector *v);
    double (*find_intersection)(struct object *self, struct ray *r);

};

struct plane
{

    struct object base;
    struct vector normal;
    double distance;

};

struct sphere
{

    struct object base;
    struct vector center;
    double radius;

};

static double vector_magnitude(struct vector *v)
{

    return sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z));

}

static double vector_dotproduct(struct vector *v1, struct vector *v2)
{

    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;

}

static struct vector vector_normalize(struct vector *v)
{

    double magnitude = vector_magnitude(v);
    struct vector t = {v->x / magnitude, v->y / magnitude, v->z / magnitude};

    return t;

}

static struct vector vector_negative(struct vector *v)
{

    struct vector t = {-v->x, -v->y, -v->z};

    return t;

}

static struct vector vector_crossproduct(struct vector *v1, struct vector *v2)
{

    struct vector t = {v1->y * v2->z - v1->z * v2->y, v1->z * v2->x - v1->x * v2->z, v1->x * v2->y - v1->y * v2->x};

    return t;

}

static struct vector vector_add(struct vector *v1, struct vector *v2)
{

    struct vector t = {v1->x + v2->x, v1->y + v2->y, v1->z + v2->z};

    return t;

}

static struct vector vector_subtract(struct vector *v1, struct vector *v2)
{

    struct vector t = {v1->x - v2->x, v1->y - v2->y, v1->z - v2->z};

    return t;

}

static struct vector vector_multiply(struct vector *v1, double scalar)
{

    struct vector t = {v1->x * scalar, v1->y * scalar, v1->z * scalar};

    return t;

}

static struct vector plane_get_normal_at(struct object *self, struct vector *v)
{

    struct plane *p = (struct plane *)self;

    return p->normal;

}

static double plane_find_intersection(struct object *self, struct ray *r)
{

    struct plane *p = (struct plane *)self;
    double a = vector_dotproduct(&r->direction, &p->normal);
    struct vector v;

    if (!a)
        return -1;

    v = vector_multiply(&p->normal, p->distance);
    v = vector_negative(&v);
    v = vector_add(&r->origin, &v);

    return -1 * vector_dotproduct(&p->normal, &v) / a;

}

static struct vector sphere_get_normal_at(struct object *self, struct vector *v)
{

    struct sphere *s = (struct sphere *)self;
    struct vector a = vector_negative(&s->center);
    struct vector b = vector_add(v, &a);

    return vector_normalize(&b);

}

static double sphere_find_intersection(struct object *self, struct ray *r)
{

    struct sphere *s = (struct sphere *)self;

    struct vector ray_origin = r->origin;
    struct vector ray_direction = r->direction;
    struct vector sphere_center = s->center;

    double ray_origin_x = ray_origin.x;
    double ray_origin_y = ray_origin.y;
    double ray_origin_z = ray_origin.z;

    double ray_direction_x = ray_direction.x;
    double ray_direction_y = ray_direction.y;
    double ray_direction_z = ray_direction.z;

    double sphere_center_x = sphere_center.x;
    double sphere_center_y = sphere_center.y;
    double sphere_center_z = sphere_center.z;

    double a = 1;

    double b = (2 * (ray_origin_x - sphere_center_x) * ray_direction_x) + (2 * (ray_origin_y - sphere_center_y) * ray_direction_y) + (2 * (ray_origin_z - sphere_center_z) * ray_direction_z);
    double c = pow(ray_origin_x - sphere_center_x, 2) + pow(ray_origin_y - sphere_center_y, 2) + pow(ray_origin_z - sphere_center_z, 2) - (s->radius * s->radius);
    double discriminant = b * b - 4 * c;

    if (discriminant > 0)
    {

        double root_1 = ((-1 * b - sqrt(discriminant)) / 2) - 0.000001;

        if (root_1 > 0)
            return root_1;

        return ((sqrt(discriminant) - b) / 2) - 0.000001;

    }

    return -1;

}

void save(const char *name, int w, int h, int dpi, unsigned int count, struct rgbtype *data)
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

        struct rgbtype rgb = data[i];
        double r = (data[i].r) * 255;
        double g = (data[i].g) * 255;
        double b = (data[i].b) * 255;
        unsigned char color[3] = {(int)floor(b), (int)floor(g), (int)floor(r)};

        fwrite(color, 1, 3, f);

    }

    fclose(f);

}

static int winning_object_index(double *intersection, unsigned int count)
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

void render(unsigned int w, unsigned int h, struct rgbtype *data, struct camera *camera, unsigned int count, struct object *objects[])
{

    double aspectratio = (double)w / (double)h;
    unsigned int x;
    unsigned int y;
    double xamnt, yamnt;
    unsigned int index;

    for (x = 0; x < w; x++)
    {

        for (y = 0; y < h; y++)
        {

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

            struct vector cam_ray_origin = camera->position;
            struct vector cam_ray_direction;
            struct vector a, b, c;

            a = vector_multiply(&camera->right, xamnt - 0.5);
            b = vector_multiply(&camera->down, yamnt - 0.5);
            c = vector_add(&a, &b);
            c = vector_add(&camera->direction, &c);
            cam_ray_direction = vector_normalize(&c);

            struct ray cam_ray = {cam_ray_origin, cam_ray_direction};

            double intersections[2000];

            for (index = 0; index < count; index++)
            {

                intersections[index] = objects[index]->find_intersection(objects[index], &cam_ray);

            }

            int index_of_winning_object = winning_object_index(intersections, 2);

            unsigned int current = y * w + x;

            if (index_of_winning_object == -1)
            {

                data[current].r = 0;
                data[current].g = 0;
                data[current].b = 0;

            }

            else
            {

                struct color this_color = objects[index_of_winning_object]->color;

                data[current].r = this_color.r;
                data[current].g = this_color.g;
                data[current].b = this_color.b;

               

            }

        }

    }

}

int main(int argc, char **argv)
{

    unsigned int w = 640;
    unsigned int h = 480;
    unsigned int dpi = 72;
    struct rgbtype *data = malloc(sizeof (struct rgbtype) * w * h);
    struct vector origin = {0.0, 0.0, 0.0};
    struct vector x = {1.0, 0.0, 0.0};
    struct vector y = {0.0, 1.0, 0.0};
    struct vector z = {0.0, 0.0, 1.0};
    struct vector lookat = {0.0, 0.0, 0.0};
    struct vector campos = {3.0, 1.5, -4.0};
    struct vector diff_btw = vector_subtract(&campos, &lookat);

    struct vector camdir = vector_negative(&diff_btw);

    camdir = vector_normalize(&camdir);

    struct vector camright = vector_crossproduct(&y, &camdir);

    camright = vector_normalize(&camright);

    struct vector camdown = vector_crossproduct(&camright, &camdir);
    struct camera scene_cam = {campos, camdir, camright, camdown};

    struct color white_light = {1.0, 1.0, 1.0, 0.0};
    struct color pretty_green = {0.5, 1.0, 0.5, 0.3};
    struct color gray = {0.5, 0.5, 0.5, 0.0};
    struct color black = {0.0, 0.0, 0.0, 0.0};
    struct color maroon = {0.5, 0.25, 0.25, 0.0};

    struct vector light_position = {-7.0, 10.0, -10.0};
    struct light scene_light = {light_position, white_light};

    struct sphere scene_sphere = {{pretty_green, sphere_get_normal_at, sphere_find_intersection}, origin, 1.0};
    struct plane scene_plane = {{maroon, plane_get_normal_at, plane_find_intersection}, y, -1.0};
    struct object *scene_objects[] = {&scene_sphere.base, &scene_plane.base};

    render(w, h, data, &scene_cam, 2, scene_objects);
    save("scene.bmp", w, h, dpi, w * h, data);
    free(data);

    return 0;

}

