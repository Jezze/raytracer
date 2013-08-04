struct bmp_color
{

    double r;
    double g;
    double b;

};

void bmp_save(const char *name, int w, int h, int dpi, unsigned int count, struct bmp_color *data);
