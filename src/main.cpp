#include <iostream>
#include <vector>

#include "SDL.h"
#include "Qualum.h"
#include "Rendering.h"
#include "Color.h"

SDL_Window* window;

float TIME_SCALE = 4.0f;

void quit()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void read_file(char* filename, int x, int y, int speedx, int speedy)
{
    // Create the qualums from the given file
    // Increasing their positions by (x,y) and their speeds by (speedx, speedy)
    FILE* input_file = fopen(filename, "r"); // Skip first line
    if (!input_file)
    {    
        fprintf(stderr, "File not found: %s\n", filename);
        exit(1);
    }
    char* buffer = (char*)malloc(4096);
    fgets(buffer, 4096, input_file);
    
    int xrel, yrel, speedxrel, speedyrel; // Read input file
    char* color_str = (char*) malloc(4096);
    char* include_filename = (char*) malloc(4096); // File name for includes
    
    while (fgets(buffer, 4096, input_file))
    {
        int i;
        for (i = 0; i < strlen(buffer); i++)
            if (buffer[i] == '#')
                break;
        buffer[i] = 0;
        if (sscanf(buffer, "%d %d %d %d %s", &xrel, &yrel, &speedxrel, &speedyrel, color_str) > 0)
        {
            if (color_str[0] == '!') // Include
            {
                int c;
                for (c = 0; color_str[c+1]; c++) // Remove !
                    include_filename[c] = color_str[c+1];
                include_filename[c] = 0;
                read_file(include_filename, x + xrel, y + yrel, speedx + speedxrel, speedy + speedyrel);
            }
            else // Color
            {
                Color color = Colors::read_color(color_str);
                Qualum::create_qualum(x + xrel, y + yrel, color, speedx + speedxrel, speedy + speedyrel);
            }
        }
    }
}

int main(int argc, char** argv) 
{
    if (argc < 2)
    {
        fprintf(stderr, "Error: No input file provided.\n");
        return 1;
    }
    
    Qualum::initialize();
    window = SDL_CreateWindow("Qualums", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, Rendering::WIDTH, Rendering::HEIGHT,
        SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_Event event;
    bool will_quit = false;
    
    read_file(argv[1], 0, 0, 0, 0);
    
    int i = 0, last_printed = -1;
    while (!will_quit)
    {
        i++;
        if (Qualum::iterations != last_printed)
        {
            printf("%d\n", Qualum::iterations);
            last_printed = Qualum::iterations;
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        Qualum::update_all(i % ((int)(300/TIME_SCALE)) == 0);
        Qualum::render_all(renderer);
        SDL_RenderPresent(renderer);
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    will_quit = true;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9)
                    {
                        TIME_SCALE = (float)(event.key.keysym.sym - SDLK_0);
                        break;
                    }
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_SPACE:
                            Qualum::update_all(true);
                            break;
                        case SDLK_ESCAPE:
                            will_quit = true;
                            break;
                    }
                    break;
            }
        }
    }
    
    return 0;
}

