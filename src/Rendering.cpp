#include "Rendering.h"

int Rendering::RENDER_SCALE = 3;
int Rendering::GRID_WIDTH   = 400;
int Rendering::GRID_HEIGHT  = 300;
int Rendering::WIDTH        = GRID_WIDTH * RENDER_SCALE;
int Rendering::HEIGHT       = GRID_HEIGHT * RENDER_SCALE;

void Rendering::set_grid_width(int new_value)
{
    GRID_WIDTH = new_value;
    WIDTH = GRID_WIDTH * RENDER_SCALE;
}

void Rendering::set_grid_height(int new_value)
{
    GRID_HEIGHT = new_value;
    HEIGHT = GRID_HEIGHT * RENDER_SCALE;
}

void Rendering::set_render_scale(int new_value)
{
    RENDER_SCALE = new_value;
    WIDTH = GRID_WIDTH * RENDER_SCALE;
    HEIGHT = GRID_HEIGHT * RENDER_SCALE;
}