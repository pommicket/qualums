#ifndef RENDERING_H
#define RENDERING_H

class Rendering {
public:
    static int RENDER_SCALE;
    static int GRID_WIDTH;
    static int GRID_HEIGHT;
    static int WIDTH;
    static int HEIGHT;
    static void set_grid_width(int new_value);
    static void set_grid_height(int new_value);
    static void set_render_scale(int new_value);
private:
};

#endif /* RENDERING_H */

