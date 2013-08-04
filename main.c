#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "vector3.h"
#include "color.h"
#include "bmp.h"

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

struct source
{

    struct color color;
    struct vector3 position;

};

struct entity
{

    struct color color;
    struct vector3 (*get_normal_at)(struct entity *self, struct vector3 *v);
    double (*find_intersection)(struct entity *self, struct ray *r);

};

struct light
{

    struct source base;

};

struct plane
{

    struct entity base;
    struct vector3 normal;
    double distance;

};

struct sphere
{

    struct entity base;
    struct vector3 center;
    double radius;

};

static struct vector3 plane_get_normal_at(struct entity *self, struct vector3 *v)
{

    struct plane *p = (struct plane *)self;

    return p->normal;

}

static double plane_find_intersection(struct entity *self, struct ray *r)
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

static struct vector3 sphere_get_normal_at(struct entity *self, struct vector3 *v)
{

    struct sphere *s = (struct sphere *)self;
    struct vector3 a = vector3_negative(&s->center);
    struct vector3 b = vector3_add(v, &a);

    return vector3_normalize(&b);

}

static double sphere_find_intersection(struct entity *self, struct ray *r)
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

static struct color get_color_at(struct vector3 *intersection_position, struct vector3 *intersecting_ray_direction, int index, int nentities, struct entity *entities[], int nlights, struct source *lights[], double accuracy, double ambientlight)
{

    double intersections[128];
    struct color winning_entity_color = entities[index]->color;
    struct vector3 winning_entity_normal = entities[index]->get_normal_at(entities[index], intersection_position);
    unsigned int i;

    if (winning_entity_color.special == 2)
    {

        int square = (int)floor(intersection_position->x) + (int)floor(intersection_position->z);

        if ((square % 2) == 0)
        {

            winning_entity_color.r = 0.0;
            winning_entity_color.g = 0.0;
            winning_entity_color.b = 0.0;

        }

        else
        {

            winning_entity_color.r = 1.0;
            winning_entity_color.g = 1.0;
            winning_entity_color.b = 1.0;

        }

    }

    struct color final_color = color_scalar(&winning_entity_color, ambientlight);

    if (winning_entity_color.special > 0 && winning_entity_color.special <= 1)
    {

        struct vector3 x = vector3_negative(intersecting_ray_direction);
        double dot1 = vector3_dotproduct(&winning_entity_normal, &x);
        struct vector3 scalar1 = vector3_multiply(&winning_entity_normal, dot1);
        struct vector3 add1 = vector3_add(&scalar1, intersecting_ray_direction);
        struct vector3 scalar2 = vector3_multiply(&add1, 2);
        struct vector3 add2 = vector3_add(&x, &scalar2);
        struct vector3 reflection_direction = vector3_normalize(&add2);
        unsigned int j;

        struct ray reflection_ray = {*intersection_position, reflection_direction};

        for (j = 0; j < nentities; j++)
            intersections[j] = entities[j]->find_intersection(entities[j], &reflection_ray);

        int index2 = find_closest(intersections, nentities);

        if (index2 != -1)
        {

            if (intersections[index2] > accuracy)
            {

                struct vector3 p = vector3_multiply(&reflection_direction, intersections[index2]);
                struct vector3 reflection_intersection_position = vector3_add(intersection_position, &p);
                struct vector3 reflection_intersecting_ray_direction = reflection_direction;
                struct color reflection_color = get_color_at(&reflection_intersection_position, &reflection_intersecting_ray_direction, index2, nentities, entities, nlights, lights, accuracy, ambientlight);
                struct color c2 = color_scalar(&reflection_color, winning_entity_color.special);

                final_color = color_add(&final_color, &c2);

            }

        }

    }

    for (i = 0; i < nlights; i++)
    {

        struct vector3 position = lights[i]->position;
        struct vector3 a = vector3_negative(intersection_position);
        struct vector3 light_direction = vector3_add(&position, &a);
        unsigned int j;

        light_direction = vector3_normalize(&light_direction);

        double cosine_angle = vector3_dotproduct(&winning_entity_normal, &light_direction);

        if (cosine_angle > 0)
        {

            unsigned int shadowed = 0;
            double magnitude = vector3_magnitude(&light_direction);
            struct vector3 f = vector3_add(&position, &light_direction);
            struct ray shadow_ray = {*intersection_position, vector3_normalize(&f)};

            for (j = 0; j < nentities && shadowed == 0; j++)
                intersections[j] = entities[j]->find_intersection(entities[j], &shadow_ray);

            for (j = 0; j < nentities; j++)
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

                struct color wc = color_multiply(&winning_entity_color, &lights[i]->color);

                wc = color_scalar(&wc, cosine_angle);

                final_color = color_add(&final_color, &wc);

                if (winning_entity_color.special > 0 && winning_entity_color.special <= 1)
                {

                    struct vector3 y = vector3_negative(intersecting_ray_direction);
                    double dot1 = vector3_dotproduct(&winning_entity_normal, &y);
                    struct vector3 scalar1 = vector3_multiply(&winning_entity_normal, dot1);
                    struct vector3 add1 = vector3_add(&scalar1, intersecting_ray_direction);
                    struct vector3 scalar2 = vector3_multiply(&add1, 2);
                    struct vector3 add2 = vector3_add(&y, &scalar2);
                    struct vector3 reflection_direction = vector3_normalize(&add2);
                    double specular = vector3_dotproduct(&reflection_direction, &light_direction);

                    if (specular > 0)
                    {

                        struct color shine = color_scalar(&lights[i]->color, pow(specular, 10) * winning_entity_color.special);

                        final_color = color_add(&final_color, &shine);

                    }

                }

            }

        }

    }

    color_clip(&final_color);

    return final_color;

}

void render(unsigned int w, unsigned int h, struct bmp_color *data, struct camera *camera, unsigned int nentities, struct entity *entities[], unsigned int nlights, struct source *lights[])
{

    double intersections[128];
    struct color black = {0.0, 0.0, 0.0, 0.0};
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

            for (i = 0; i < nentities; i++)
                intersections[i] = entities[i]->find_intersection(entities[i], &camray);

            int index = find_closest(intersections, nentities);

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
                    struct color intersection_color = get_color_at(&intersection_position, &camray.direction, index, nentities, entities, nlights, lights, accuracy, ambientlight);

                    current->r = intersection_color.r;
                    current->g = intersection_color.g;
                    current->b = intersection_color.b;

                }

            }

        }

    }

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
    struct vector3 new_sphere_location = {1.75, 0.0, 0.0};
    struct color white = {1.0, 1.0, 1.0, 0.0};
    struct color green = {0.5, 1.0, 0.5, 0.4};
    struct color pink = {1.0, 0.5, 1.0, 0.4};
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

    struct light light00 = {{white, {-7.0, 10.0, -10.0}}};
    struct sphere sphere00 = {{green, sphere_get_normal_at, sphere_find_intersection}, origin, 1.0};
    struct sphere sphere01 = {{pink, sphere_get_normal_at, sphere_find_intersection}, new_sphere_location, 0.5};
    struct plane plane00 = {{brown, plane_get_normal_at, plane_find_intersection}, originy, -1.0};

    struct entity *entities[] = {&sphere00.base, &sphere01.base, &plane00.base};
    struct source *lights[] = {&light00.base};

    render(w, h, data, &camera, 3, entities, 1, lights);
    bmp_save("scene.bmp", w, h, dpi, w * h, data);
    free(data);

    return 0;

}

