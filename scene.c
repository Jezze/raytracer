#include <stdlib.h>
#include <math.h>
#include "vector3.h"
#include "color.h"
#include "material.h"
#include "ray.h"
#include "entity.h"
#include "camera.h"
#include "plane.h"
#include "sphere.h"
#include "light.h"
#include "backend.h"
#include "scene.h"

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

void scene_fill_intersections(struct scene *scene, struct ray *ray, double *intersections)
{

    unsigned int i;

    for (i = 0; i < scene->entities.count; i++)
        intersections[i] = scene->entities.items[i]->find_intersection(scene->entities.items[i], ray);

}

struct color scene_get_color(struct scene *scene, struct ray *r, struct entity *entity)
{

    double intersections[128];
    double accuracy = 0.000001;
    struct vector3 normal = entity->find_normal(entity, &r->origin);
    struct color color = entity->material.diffuse;
    unsigned int i;

    if (entity->material.type == MATERIAL_TYPE_CHECKERS)
        material_checkers(&entity->material, &r->origin);

    color_scalar(&color, scene->ambientlight);

    if (entity->material.reflection > 0 && entity->material.reflection <= 1)
    {

        struct vector3 scalar = normal;
        struct ray ray = *r;
        int index;

        vector3_negative(&ray.direction);
        vector3_scalar(&scalar, vector3_dotproduct(&scalar, &ray.direction));
        vector3_add(&scalar, &r->direction);
        vector3_scalar(&scalar, 2);
        vector3_add(&ray.direction, &scalar);
        vector3_normalize(&ray.direction);
        scene_fill_intersections(scene, &ray, intersections);

        index = find_closest(intersections, scene->entities.count);

        if (index != -1)
        {

            struct ray rr = {ray.direction, ray.direction};
            struct color c;

            vector3_scalar(&rr.origin, intersections[index]);
            vector3_add(&rr.origin, &r->origin);

            c = scene_get_color(scene, &rr, scene->entities.items[index]);

            color_scalar(&c, entity->material.reflection);
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
            scene_fill_intersections(scene, &ray, intersections);

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

                struct color c = entity->material.diffuse;

                color_multiply(&c, &scene->lights.items[i]->color);
                color_scalar(&c, angle);
                color_add(&color, &c);

                if (entity->material.reflection > 0 && entity->material.reflection <= 1)
                {

                    struct vector3 scalar = normal;
                    struct vector3 y = r->direction;
                    double specular;

                    vector3_negative(&y);
                    vector3_scalar(&scalar, vector3_dotproduct(&scalar, &y));
                    vector3_add(&scalar, &r->direction);
                    vector3_scalar(&scalar, 2);
                    vector3_add(&y, &scalar);
                    vector3_normalize(&y);

                    specular = vector3_dotproduct(&y, &direction);

                    if (specular > 0)
                    {

                        struct color shine = scene->lights.items[i]->color;

                        color_scalar(&shine, pow(specular, 10) * entity->material.reflection);
                        color_add(&color, &shine);

                    }

                }

            }

        }

    }

    color_clip(&color);

    return color;

}

void scene_render(struct scene *scene, struct backend *backend, struct color *data)
{

    double intersections[128];
    struct color black = {0.0, 0.0, 0.0};
    double aspectratio = (double)backend->w / (double)backend->h;
    unsigned int diff = backend->w - backend->h;
    unsigned int x;
    unsigned int y;
    unsigned int i;

    for (x = 0; x < backend->w; x++)
    {

        for (y = 0; y < backend->h; y++)
        {

            struct color *current = &data[y * backend->w + x];
            struct color color = black;
            struct ray ray = {scene->camera.position, scene->camera.right};
            struct vector3 b = scene->camera.down;
            double xa = ((x + 0.5) / backend->w);
            double ya = (((backend->h - y) + 0.5) / backend->h);
            int index;

            if (diff > 0)
                xa = xa * aspectratio - (diff / (double)backend->h / 2.0);
            else
                ya = ya / aspectratio + (diff / (double)backend->w / 2.0);

            vector3_scalar(&ray.direction, xa - 0.5);
            vector3_scalar(&b, ya - 0.5);
            vector3_add(&ray.direction, &b);
            vector3_add(&ray.direction, &scene->camera.direction);
            vector3_normalize(&ray.direction);
            scene_fill_intersections(scene, &ray, intersections);

            index = find_closest(intersections, scene->entities.count);

            if (index != -1)
            {

                vector3_scalar(&ray.direction, intersections[index]);
                vector3_add(&ray.origin, &ray.direction);

                color = scene_get_color(scene, &ray, scene->entities.items[index]);

            }

            current->r = color.r;
            current->g = color.g;
            current->b = color.b;

        }

    }

}

