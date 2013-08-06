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
#include "bmp.h"

struct
{

    struct backend base;
    unsigned int dpi;

} backend;

static void start()
{

}

static void stop()
{

}

static void render(struct scene *scene, struct color *data)
{

    FILE *f;
    char *name = "scene.bmp";
    unsigned int count = backend.base.w * backend.base.h;
    unsigned int s = 4 * count;
    unsigned int filesize = 54 + s;
    unsigned int i;
    double factor = 39.375;
    int m = (int)factor;
    int ppm = backend.dpi * m;
    unsigned char bmpfileheader[14] = {'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0};
    unsigned char bmpinfoheader[40] = {40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0};

    bmpfileheader[2] = (unsigned char)(filesize);
    bmpfileheader[3] = (unsigned char)(filesize >> 8);
    bmpfileheader[4] = (unsigned char)(filesize >> 16);
    bmpfileheader[5] = (unsigned char)(filesize >> 24);

    bmpinfoheader[4] = (unsigned char)(backend.base.w);
    bmpinfoheader[5] = (unsigned char)(backend.base.w >> 8);
    bmpinfoheader[6] = (unsigned char)(backend.base.w >> 16);
    bmpinfoheader[7] = (unsigned char)(backend.base.w >> 24);

    bmpinfoheader[8] = (unsigned char)(backend.base.h);
    bmpinfoheader[9] = (unsigned char)(backend.base.h >> 8);
    bmpinfoheader[10] = (unsigned char)(backend.base.h >> 16);
    bmpinfoheader[11] = (unsigned char)(backend.base.h >> 24);

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

        struct color rgb = data[i];
        double r = (data[i].r) * 255;
        double g = (data[i].g) * 255;
        double b = (data[i].b) * 255;
        unsigned char color[3];
        color[0] = (int)floor(b);
        color[1] = (int)floor(g);
        color[2] = (int)floor(r);

        fwrite(color, 1, 3, f);

    }

    fclose(f);

}

struct backend *bmp_init(unsigned int w, unsigned int h)
{

    backend.base.w = w;
    backend.base.h = h;
    backend.base.start = start;
    backend.base.stop = stop;
    backend.base.render = render;
    backend.dpi = 72;

    return &backend.base;

}

