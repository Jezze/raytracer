struct vector3
{

    double x;
    double y;
    double z;

};

double vector3_magnitude(struct vector3 *v);
double vector3_dotproduct(struct vector3 *v1, struct vector3 *v2);
struct vector3 vector3_normalize(struct vector3 *v);
struct vector3 vector3_negative(struct vector3 *v);
struct vector3 vector3_crossproduct(struct vector3 *v1, struct vector3 *v2);
struct vector3 vector3_add(struct vector3 *v1, struct vector3 *v2);
struct vector3 vector3_subtract(struct vector3 *v1, struct vector3 *v2);
struct vector3 vector3_multiply(struct vector3 *v1, double scalar);
