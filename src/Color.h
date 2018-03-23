
#ifndef COLOR_H
#define COLOR_H

enum class Color { GRAY, RED, BLUE, GREEN, PURPLE, YELLOW, CYAN, ORANGE };

class Colors {
public:
    static Color read_color(char* s);
    static void to_rgb(Color c, unsigned char* r, unsigned char* g, unsigned char* b);
private:
    static unsigned get_color(Color c);
    static unsigned colors[];
    static char*    color_names[];
};

#endif /* COLOR_H */

