struct backend
{

    unsigned int w;
    unsigned int h;
    void (*start)();
    void (*stop)();
    void (*render)(struct scene *scene, struct color *color);

};
