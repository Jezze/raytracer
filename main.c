#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include "vector3.h"
#include "color.h"
#include "material.h"
#include "ray.h"
#include "entity.h"
#include "camera.h"
#include "plane.h"
#include "sphere.h"
#include "light.h"
#include "scene.h"
#include "backend.h"
#include "bmp.h"
#include "glfw.h"

enum output
{

    OUTPUT_BMP                          = 1,
    OUTPUT_GLFW                         = 2

};

void usage()
{

    fputs("usage: raytracer WIDTH HEIGHT\n", stderr);
    exit(EXIT_FAILURE);

}

int main(int argc, char **argv)
{

    enum output output = OUTPUT_GLFW;
    unsigned int width = 640;
    unsigned int height = 480;
    static struct scene scene;
    struct backend *backend;
    struct vector3 origin = {0.0, 0.0, 0.0};
    struct vector3 originy = {0.0, 1.0, 0.0};
    struct light light00 = {{1.0, 1.0, 1.0}, {-10.0, 10.0, -10.0}};
    struct sphere sphere00 = {{{MATERIAL_TYPE_NORMAL, {0.5, 1.0, 0.5}, 0.4, 0.0}, sphere_find_normal, sphere_find_intersection}, {-1.0, 0.0, 0.0}, 1.0};
    struct sphere sphere01 = {{{MATERIAL_TYPE_NORMAL, {1.0, 0.5, 1.0}, 0.4, 0.0}, sphere_find_normal, sphere_find_intersection}, {2.0, -0.5, 1.5}, 0.5};
    struct plane plane00 = {{{MATERIAL_TYPE_CHECKERS, {0.5, 0.25, 0.25}, 0.0, 0.0}, plane_find_normal, plane_find_intersection}, {0.0, 1.0, 0.0}, -1.0};
    struct color *data;

    scene.ambientlight = 0.1;
    scene.entities.items[0] = &sphere00.base;
    scene.entities.items[1] = &sphere01.base;
    scene.entities.items[2] = &plane00.base;
    scene.entities.count = 3;
    scene.lights.items[0] = &light00;
    scene.lights.count = 1;

    camera_init(&scene.camera, &origin, &originy);

    if (argc < 3)
        usage();

    width = strtoul(argv[1], 0, 10);

    if (!width)
        usage();

    height = strtoul(argv[2], 0, 10);

    if (!height)
        usage();

    data = malloc(sizeof (struct color) * width * height);

    switch (output)
    {

        case OUTPUT_BMP:

            backend = bmp_init(width, height);

            break;

        case OUTPUT_GLFW:

            backend = glfw_init(width, height);

            break;

        default:

            backend = glfw_init(width, height);

            break;

    }

    backend->start();
    backend->render(&scene, data);
    backend->stop();
    free(data);

    return 0;

}

