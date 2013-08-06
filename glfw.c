#include <stdlib.h>
#include <stdio.h>
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
#include "glfw.h"

struct
{

    struct backend base;
    GLFWwindow *window;

} backend;

/*
static void glfw_error(int error, const char* description)
{

    fputs(description, stderr);
    
}
*/

static void start()
{

    if (!glfwInit())
        exit(EXIT_FAILURE);

    backend.window = glfwCreateWindow(backend.base.w, backend.base.h, "Raytracer", 0, 0);

    if (!backend.window)
    {

        glfwTerminate();
        exit(EXIT_FAILURE);

    }

    glfwMakeContextCurrent(backend.window);

}

static void stop()
{

    glfwTerminate();

}

static void render(struct scene *scene, struct color *data)
{

    double speedx = 0.01;
    double speedy = 0.01;

    while (!glfwWindowShouldClose(backend.window))
    {

        scene->camera.direction.x += speedx;
        scene->camera.direction.y += speedy;

        if (scene->camera.direction.x >= 0.4 || scene->camera.direction.x <= -0.4)
            speedx = -speedx;

        if (scene->camera.direction.y >= 0.4 || scene->camera.direction.y <= -0.4)
            speedy = -speedy;

        scene_render(scene, backend.base.w, backend.base.h, data);

        glDrawPixels(backend.base.w, backend.base.h, GL_RGB, GL_FLOAT, data);
        glfwSwapBuffers(backend.window);
        glfwPollEvents();

    }

}

struct backend *glfw_init(unsigned int w, unsigned int h)
{

    backend.base.w = w;
    backend.base.h = h;
    backend.base.start = start;
    backend.base.stop = stop;
    backend.base.render = render;

    return &backend.base;

}

