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
    double reflection;
    unsigned int texture;
    struct vector3 (*find_normal)(struct entity *self, struct vector3 *v);
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

struct scene
{

    double ambientlight;
    struct camera camera;
    struct {struct entity *items[128]; unsigned int count;} entities;
    struct {struct source *items[128]; unsigned int count;} lights;

};

struct backend
{

    unsigned int w;
    unsigned int h;
    unsigned int dpi;

};

static struct vector3 plane_find_normal(struct entity *self, struct vector3 *v)
{

    struct plane *p = (struct plane *)self;

    return p->normal;

}

static double plane_find_intersection(struct entity *self, struct ray *r)
{

    struct plane *p = (struct plane *)self;
    double a = vector3_dotproduct(&r->direction, &p->normal);
    struct vector3 v = p->normal;

    if (!a)
        return -1;

    vector3_scalar(&v, p->distance);
    vector3_negative(&v);
    vector3_add(&v, &r->origin);

    return -1 * vector3_dotproduct(&p->normal, &v) / a;

}

static struct vector3 sphere_find_normal(struct entity *self, struct vector3 *v)
{

    struct sphere *s = (struct sphere *)self;
    struct vector3 a = s->center;
    struct vector3 b = *v;

    vector3_negative(&a);
    vector3_add(&b, &a);
    vector3_normalize(&b);

    return b;

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

static void texture_checkerboard(struct entity *entity, struct vector3 *position)
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

static void fill_intersections(double *intersections, struct scene *scene, struct ray *ray)
{

    unsigned int i;

    for (i = 0; i < scene->entities.count; i++)
        intersections[i] = scene->entities.items[i]->find_intersection(scene->entities.items[i], ray);

}

static struct color get_color_at(struct ray *r, struct scene *scene, struct entity *entity)
{

    double intersections[128];
    double accuracy = 0.000001;
    struct vector3 normal = entity->find_normal(entity, &r->origin);
    struct color color = entity->color;
    unsigned int i;

    if (entity->texture == 1)
        texture_checkerboard(entity, &r->origin);

    color_scalar(&color, scene->ambientlight);

    if (entity->reflection > 0 && entity->reflection <= 1)
    {

        struct vector3 scalar = normal;
        struct ray ray = *r;

        vector3_negative(&ray.direction);
        vector3_scalar(&scalar, vector3_dotproduct(&scalar, &ray.direction));
        vector3_add(&scalar, &r->direction);
        vector3_scalar(&scalar, 2);
        vector3_add(&ray.direction, &scalar);
        vector3_normalize(&ray.direction);
        fill_intersections(intersections, scene, &ray);

        int index = find_closest(intersections, scene->entities.count);

        if (index != -1)
        {

            struct ray rr = {ray.direction, ray.direction};
            struct color c;

            vector3_scalar(&rr.origin, intersections[index]);
            vector3_add(&rr.origin, &r->origin);

            c = get_color_at(&rr, scene, scene->entities.items[index]);
            color_scalar(&c, entity->reflection);
            color_add(&color, &c);

        }

    }

    for (i = 0; i < scene->lights.count; i++)
    {

        struct vector3 a = r->origin;
        struct vector3 direction = scene->lights.items[i]->position;
        double angle;
        unsigned int j;

        vector3_negative(&a);
        vector3_add(&direction, &a);
        vector3_normalize(&direction);

        angle = vector3_dotproduct(&normal, &direction);

        if (angle > 0)
        {

            struct ray ray = {r->origin, scene->lights.items[i]->position};
            unsigned int shadowed = 0;

            vector3_add(&ray.direction, &direction);
            vector3_normalize(&ray.direction);
            fill_intersections(intersections, scene, &ray);

            for (j = 0; j < scene->entities.count; j++)
            {

                if (intersections[j] > accuracy)
                {

                    if (intersections[j] <= vector3_magnitude(&direction))
                        shadowed = 1;

                    break;

                }

            }

            if (shadowed == 0)
            {

                struct color c = entity->color;

                color_multiply(&c, &scene->lights.items[i]->color);
                color_scalar(&c, angle);
                color_add(&color, &c);

                if (entity->reflection > 0 && entity->reflection <= 1)
                {

                    struct vector3 scalar = normal;
                    struct vector3 y = r->direction;

                    vector3_negative(&y);
                    vector3_scalar(&scalar, vector3_dotproduct(&scalar, &y));
                    vector3_add(&scalar, &r->direction);
                    vector3_scalar(&scalar, 2);
                    vector3_add(&y, &scalar);
                    vector3_normalize(&y);

                    double specular = vector3_dotproduct(&y, &direction);

                    if (specular > 0)
                    {

                        struct color shine = scene->lights.items[i]->color;

                        color_scalar(&shine, pow(specular, 10) * entity->reflection);
                        color_add(&color, &shine);

                    }

                }

            }

        }

    }

    color_clip(&color);

    return color;

}

void render(struct scene *scene, struct backend *backend, struct bmp_color *data)
{

    double intersections[128];
    struct color black = {0.0, 0.0, 0.0};
    double aspectratio = (double)backend->w / (double)backend->h;
    unsigned int x;
    unsigned int y;
    double xa;
    double ya;
    unsigned int i;

    for (x = 0; x < backend->w; x++)
    {

        for (y = 0; y < backend->h; y++)
        {

            struct bmp_color *current = &data[y * backend->w + x];

            if (backend->w > backend->h)
            {

                xa = ((x + 0.5) / backend->w) * aspectratio - (((backend->w - backend->h) / (double)backend->h) / 2);
                ya = ((backend->h - y) + 0.5) / backend->h;

            }

            else if (backend->h > backend->w)
            {

                xa = (x + 0.5) / backend->w;
                ya = (((backend->h - y) + 0.5) / backend->h) / aspectratio - (((backend->h - backend->w) / (double)backend->w) / 2);

            }

            else
            {

                xa = (x + 0.5) / backend->w;
                ya = ((backend->h - y) + 0.5) / backend->h;

            }

            struct ray camray;
            struct vector3 b = scene->camera.down;

            camray.origin = scene->camera.position;
            camray.direction = scene->camera.right;

            vector3_scalar(&camray.direction, xa - 0.5);
            vector3_scalar(&b, ya - 0.5);
            vector3_add(&camray.direction, &b);
            vector3_add(&camray.direction, &scene->camera.direction);
            vector3_normalize(&camray.direction);
            fill_intersections(intersections, scene, &camray);

            int index = find_closest(intersections, scene->entities.count);

            if (index == -1)
            {

                current->r = black.r;
                current->g = black.g;
                current->b = black.b;

            }

            else
            {

                struct color color;

                vector3_scalar(&camray.direction, intersections[index]);
                vector3_add(&camray.origin, &camray.direction);

                color = get_color_at(&camray, scene, scene->entities.items[index]);

                current->r = color.r;
                current->g = color.g;
                current->b = color.b;

            }

        }

    }

}

void setup_camera(struct camera *camera, struct vector3 *origin, struct vector3 *originy)
{

    struct vector3 position = {3.0, 1.5, -4.0};
    struct vector3 direction = position;
    struct vector3 right;
    struct vector3 down;

    vector3_subtract(&direction, origin);
    vector3_negative(&direction);
    vector3_normalize(&direction);
    right = vector3_crossproduct(originy, &direction);
    vector3_normalize(&right);
    down = vector3_crossproduct(&right, &direction);

    camera->position = position;
    camera->direction = direction;
    camera->right = right;
    camera->down = down;

}

int main(int argc, char **argv)
{

    struct backend backend;
    struct scene scene;
    struct vector3 origin = {0.0, 0.0, 0.0};
    struct vector3 originy = {0.0, 1.0, 0.0};
    struct light light00 = {{{1.0, 1.0, 1.0}, {-10.0, 10.0, -10.0}}};
    struct sphere sphere00 = {{{0.5, 1.0, 0.5}, 0.4, 0, sphere_find_normal, sphere_find_intersection}, {-1.0, 0.0, 0.0}, 1.0};
    struct sphere sphere01 = {{{1.0, 0.5, 1.0}, 0.4, 0, sphere_find_normal, sphere_find_intersection}, {2.0, -0.5, 2.0}, 0.5};
    struct plane plane00 = {{{0.5, 0.25, 0.25}, 0.0, 1, plane_find_normal, plane_find_intersection}, {0.0, 1.0, 0.0}, -1.0};
    struct bmp_color *data;

    backend.w = 800;
    backend.h = 600;
    backend.dpi = 72;
    scene.ambientlight = 0.1;
    scene.entities.items[0] = &sphere00.base;
    scene.entities.items[1] = &sphere01.base;
    scene.entities.items[2] = &plane00.base;
    scene.entities.count = 3;
    scene.lights.items[0] = &light00.base;
    scene.lights.count = 1;

    setup_camera(&scene.camera, &origin, &originy);

    data = malloc(sizeof (struct bmp_color) * backend.w * backend.h);

    render(&scene, &backend, data);
    bmp_save("scene.bmp", backend.w, backend.h, backend.dpi, backend.w * backend.h, data);
    free(data);

    return 0;

}

