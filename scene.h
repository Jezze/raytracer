struct scene
{

    double ambientlight;
    struct camera camera;
    struct {struct entity *items[128]; unsigned int count;} entities;
    struct {struct light *items[128]; unsigned int count;} lights;

};

void scene_fill_intersections(struct scene *scene, struct ray *ray, double *intersections);
struct color scene_get_color(struct scene *scene, struct ray *r, struct entity *entity);
void scene_render(struct scene *scene, struct backend *backend, struct color *data);
