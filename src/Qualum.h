#ifndef QUALUM_H
#define QUALUM_H

#include <vector>
#include <SDL.h>
#include "Color.h"
#include "Rendering.h"

class Qualum {
public:
    Qualum(int x, int y, Color color, int sx, int sy);
    ~Qualum();
    static Qualum* create_qualum(int x, int y, Color color, int sx, int sy);
    void render(SDL_Renderer* renderer);
    void update();
    static std::vector<Qualum*> qualums;
    static void render_all(SDL_Renderer* renderer);
    static void update_all(bool should_update);
    static void initialize();
    static int iterations;
private:
    static int** locations;
    static void two_perpendiculars(int speedx, int speedy, int* speedx1, int* speedy1, int* speedx2, int* speedy2);
    static void combine_speeds(int speedx1, int speedy1, int speedx2, int speedy2, int* speedx, int* speedy);
    void collide_with(Qualum* q);
    void debug();
    void destroy();
    int x, y, index;
    int speedx, speedy;
    int delayed;
    bool can_collide;
    Color color;

};

#endif /* QUALUM_H */

