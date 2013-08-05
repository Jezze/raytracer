#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
#include "bmp.h"

int main(int argc, char **argv)
{

    struct backend backend;
    struct scene scene;
    struct vector3 origin = {0.0, 0.0, 0.0};
    struct vector3 originy = {0.0, 1.0, 0.0};
    struct light light00 = {{1.0, 1.0, 1.0}, {-10.0, 10.0, -10.0}};
    struct sphere sphere00 = {{{MATERIAL_TYPE_GOURAD, {0.5, 1.0, 0.5}, 0.4, 0.0}, sphere_find_normal, sphere_find_intersection}, {-1.0, 0.0, 0.0}, 1.0};
    struct sphere sphere01 = {{{MATERIAL_TYPE_GOURAD, {1.0, 0.5, 1.0}, 0.4, 0.0}, sphere_find_normal, sphere_find_intersection}, {2.0, -0.5, 1.5}, 0.5};
    struct plane plane00 = {{{MATERIAL_TYPE_CHECKERS, {0.5, 0.25, 0.25}, 0.0, 0.0}, plane_find_normal, plane_find_intersection}, {0.0, 1.0, 0.0}, -1.0};
    struct color *data;

    backend.w = 640;
    backend.h = 480;
    backend.dpi = 72;
    scene.ambientlight = 0.1;
    scene.entities.items[0] = &sphere00.base;
    scene.entities.items[1] = &sphere01.base;
    scene.entities.items[2] = &plane00.base;
    scene.entities.count = 3;
    scene.lights.items[0] = &light00;
    scene.lights.count = 1;

    camera_init(&scene.camera, &origin, &originy);

    data = malloc(sizeof (struct color) * backend.w * backend.h);

    scene_render(&scene, &backend, data);
    bmp_save("scene.bmp", backend.w, backend.h, backend.dpi, backend.w * backend.h, data);
    free(data);

    return 0;

}

