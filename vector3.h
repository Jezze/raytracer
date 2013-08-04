struct vector3
{

    double x;
    double y;
    double z;

};

double vector3_magnitude(struct vector3 *v);
double vector3_dotproduct(struct vector3 *v1, struct vector3 *v2);
void vector3_normalize(struct vector3 *v);
void vector3_negative(struct vector3 *v);
struct vector3 vector3_crossproduct(struct vector3 *v1, struct vector3 *v2);
void vector3_add(struct vector3 *v1, struct vector3 *v2);
void vector3_subtract(struct vector3 *v1, struct vector3 *v2);
void vector3_scalar(struct vector3 *v, double scalar);
