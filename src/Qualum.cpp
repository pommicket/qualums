#include "Qualum.h"
#include "Rendering.h"
#include "Color.h"
#include <SDL.h>
#include <vector>
#include <iostream>

int Qualum::locations[Rendering::GRID_HEIGHT][Rendering::GRID_WIDTH];
std::vector<Qualum*> Qualum::qualums;
int Qualum::iterations = 0;

void Qualum::initialize()
{
    for (int i = 0; i < Rendering::GRID_HEIGHT; i++)
    {
        for (int j = 0; j < Rendering::GRID_WIDTH; j++)
        {
            locations[i][j] = -1;
        }
    }
}

Qualum::Qualum(int x_, int y_, Color color_, int sx, int sy): x(x_), y(y_), color(color_), speedx(sx), speedy(sy) {
    index = Qualum::qualums.size();
    can_collide = true;
    delayed = 0;
    Qualum::qualums.push_back(this);
}

Qualum* Qualum::create_qualum(int x, int y, Color color, int sx, int sy)
{
    // printf("Position: %d,%d Color: %d Speed: %d,%d\n", x, y, (int)color, sx, sy);
    Qualum* newq = (Qualum*)calloc(1,sizeof(Qualum));
    newq = new Qualum(x, y, color, sx, sy);
    return newq;
}

Qualum::~Qualum()
{
    if (index >= 0 && index < Qualum::qualums.size())
        Qualum::qualums[index] = NULL;
    if (!(x < 0 || y < 0 || x >= Rendering::GRID_WIDTH || y >= Rendering::GRID_HEIGHT)
        && Qualum::locations[y][x] == index)
        Qualum::locations[y][x] = -1;
}

void Qualum::destroy()
{
    // Destroy this qualum
    /*printf("%p; ", Qualum::qualums[index]);
    printf("Destroying ");
    debug();
    printf("\n");
    */
    delete this;
}

void Qualum::render(SDL_Renderer* renderer)
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    Colors::to_rgb(color, &r, &g, &b);
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_Rect rect = {x*Rendering::RENDER_SCALE, y*Rendering::RENDER_SCALE, 
                     Rendering::RENDER_SCALE, Rendering::RENDER_SCALE};
    SDL_RenderFillRect(renderer, &rect);
}

void Qualum::render_all(SDL_Renderer* renderer)
{
    for (Qualum* qualum: qualums)
    {
        if (qualum != NULL)
            qualum->render(renderer);
    }
}

void Qualum::two_perpendiculars(int speedx, int speedy, int* speedx1, int* speedy1, int* speedx2, int* speedy2)
{
    // [speedx1 speedy1] is [speedx speedy] turned 90d left, and [speedx2 speedy2] is it turned 90d right
    if (speedx1 != NULL && speedy1 != NULL)
    {
        *speedx1 = speedy;
        *speedy1 = -speedx;
    }
    if (speedx2 != NULL && speedy2 != NULL)
    {
        *speedx2 = -speedy;
        *speedy2 = speedx;
    }
}

void Qualum::debug()
{
    printf("(%d: %p) Color: %d; Position: %d,%d; Speed: %d,%d", index, Qualum::qualums[index], (int)color, x, y, speedx, speedy);
}

void Qualum::collide_with(Qualum* q)
{
    /*
    printf("Collision: ");
    debug();
    printf(" and ");
    q->debug();
    printf("\n");
    */
    if (!can_collide || !q->can_collide)
    {
        return;
    }
    if (q->color == Color::RED && color == Color::RED) // Destroy
    {
        destroy();
        q->destroy();
        return;
    }
    if (color == Color::RED)
    {
        q->destroy();
        return;
    }
    if (q->color == Color::RED)
    {
        q->collide_with(this);
        return;
    }
    
    if (color == Color::CYAN)
    {
        q->delayed = 1;
        q->can_collide = false;
        return;
    }
    if (q->color == Color::CYAN)
    {
        q->collide_with(this);
        return;
    }
    if (color == Color::ORANGE)
    {
        q->delayed = 16;
        q->can_collide = false;
        return;
    }
    if (q->color == Color::ORANGE)
    {
        q->collide_with(this);
        return;
    }
    
    if (color == Color::GRAY) // Turn
    {
        Qualum::two_perpendiculars(q->speedx, q->speedy, &q->speedx, &q->speedy, NULL, NULL);
        if (q->color == Color::GRAY)
            Qualum::two_perpendiculars(speedx, speedy, &speedx, &speedy, NULL, NULL);
        return;
    }
    if (q->color == Color::GRAY)
    {
        q->collide_with(this);
        return;
    }
    
    if ((color == Color::GREEN || color == Color::BLUE)
       && (q->color == Color::GREEN || q->color == Color::BLUE)) // NAND - Green=True, Blue=False
    {
        Color out = !((color == Color::GREEN) && (q->color == Color::GREEN)) ?
            Color::GREEN : Color::BLUE;
        Qualum::create_qualum(x, y, out, speedx + q->speedx, speedy + q->speedy);
        destroy();
        q->destroy();
        return;
    }
    
    if (color == Color::PURPLE && q->color == Color::PURPLE)
    {
        destroy();
        q->destroy();
        return;
    }
    if (color == Color::PURPLE)
    {   
        Qualum* newq1 = Qualum::create_qualum(x, y, q->color, 0, 0);
        Qualum* newq2 = Qualum::create_qualum(x, y, q->color, 0, 0);
        Qualum::two_perpendiculars(q->speedx, q->speedy, &newq1->speedx, &newq1->speedy, &newq2->speedx, &newq2->speedy);
        q->destroy();
        return;
    }
    if (q->color == Color::PURPLE)
    {
        q->collide_with(this);
        return;
    }
    
    if (color == Color::YELLOW) // Stop
    {
        x -= speedx; // Reverse any movement
        y -= speedy;
        
        q->x -= q->speedx;
        q->y -= q->speedy;
        
        q->speedx = 0;
        q->speedy = 0;
        
        speedx = 0;
        speedy = 0;
        return;
    }
    
    if (q->color == Color::YELLOW)
    {
        q->collide_with(this);
        return;
    }
}

void Qualum::update()
{
    /*
    printf("Updating ");
    debug();
    printf("\n");
    */
    if (x < 0 || y < 0 || x >= Rendering::GRID_WIDTH || y >= Rendering::GRID_HEIGHT)
    {
        destroy();
        return;
    }
    if (Qualum::locations[y][x] == index)
        Qualum::locations[y][x] = -1;
    if (delayed)
    {
        delayed--;
    }
    else
    {
        x += speedx;
        y += speedy;
        if (!can_collide)
        {
            can_collide = true;
        }
    }
    if (x < 0 || y < 0 || x >= Rendering::GRID_WIDTH || y >= Rendering::GRID_HEIGHT)
    {
        destroy();
        return;
    }
    
    if (Qualum::locations[y][x] == -1)
    {
        Qualum::locations[y][x] = index;
    }
    else
    {
        collide_with(Qualum::qualums[Qualum::locations[y][x]]);
    }
}

void Qualum::update_all(bool should_update)
{
     /*// Ideally, this code should not be necessary: 
     for (int y = 0; y < Rendering::GRID_HEIGHT; y++)
        for (int x = 0; x < Rendering::GRID_WIDTH; x++)
            Qualum::locations[y][x] = -1;
    */
    if (!should_update)
        return;
    Qualum::iterations++;
    int size = qualums.size();
    for (int i = 0; i < size; i++)
    {
        if (qualums[i] != nullptr)
        {
            qualums[i]->update();
        }
    }
}