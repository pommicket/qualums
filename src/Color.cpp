
#include "Color.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

unsigned Colors::colors[] = {0x888888, 0xFF0000, 0x0000FF, 0x00FF00,
                             0x660099, 0xFFFF00, 0x00FFFF, 0xFF8800};
const char* Colors::color_names[] = {"GA", "RE", "BL", "GE", "PU", "YE", "CY", "OR", NULL};

Color Colors::read_color(char* s)
{
    for (int i = 0; Colors::color_names[i]; i++)
        if (!strcmp(Colors::color_names[i], s))
            return (Color) i;
    fprintf(stderr, "Error - Color not found: %s\n", s);
    exit(1);
}

unsigned Colors::get_color(Color c)
{
    // Color to 0xAABBCC format
    return colors[(int)c];
}

void Colors::to_rgb(Color c, unsigned char* r, unsigned char* g, unsigned char* b)
{
    unsigned c_int = get_color(c);
    *r = c_int >> 16;
    *g = (c_int >> 8) % 256;
    *b = c_int % 256;
}