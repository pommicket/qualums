#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "SDL.h"
#include "Qualum.h"
#include "Rendering.h"
#include "Color.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
const char SLASH = '\\';
#else
const char SLASH = '/';
#endif

SDL_Window* window;

int global_file_identifier = 0; // What the global file count is at (used for creating local variables)
double TIME_SCALE = 4.0;
std::map<std::string, std::string> variables;

void quit()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

char* empty_string()
{
    // To avoid write-string warnings
    char* s = (char*)malloc(1);
    *s = 0;
    return s;
}

char* getdirname(char* filename)
{
    // Gets, e.g. a/b/ from a/b/c.txt
    char* dir = (char*)malloc(strlen(filename)+1);
    strcpy(dir, filename);
    char* lastSlash = strrchr(dir, SLASH);
    if (lastSlash)
        *(lastSlash+1) = 0;
    else
        return empty_string();
    return dir;
}

char* getfilename(char* path)
{
    char* filename = (char*)malloc(strlen(path)+1);
    strcpy(filename, path);
    char* lastSlash = strrchr(path, SLASH);
    if (lastSlash)
        return lastSlash + 1;
    return filename;
}

void str_tolower(char* str)
{
    int i;
    for (i = 0; str[i]; i++)
        str[i] = tolower(str[i]);
        
}

void set_property(const char* property, const char* value)
{
    int int_value;
    double double_value;
    if (!strcmp(property, "width") && (int_value = atoi(value)))
        Rendering::set_grid_width(int_value);
    else if (!strcmp(property, "height") && (int_value = atoi(value)))
        Rendering::set_grid_height(int_value);
    else if (!strcmp(property, "scale") && (int_value = atoi(value)))
        Rendering::set_render_scale(int_value);
    else if (!strcmp(property, "speed"))
    {
        TIME_SCALE = double_value;
    }
    variables.insert(std::make_pair<std::string,std::string>(std::string(property), std::string(value)));
}

const char* get_property(const char* property)
{
    try
    {
        return variables.at(property).c_str();
    }
    catch (...)
    {
        fprintf(stderr, "Variable not found: %s", property);
        exit(1);
    }
}

void read_file(char* directory, char* filename, int x, int y, int speedx, int speedy)
{
    // Create the qualums from the given file
    // Increasing their positions by (x,y) and their speeds by (speedx, speedy)
    
    int file_identifier = global_file_identifier++; // Used for creating local variables
    
    char* path = (char*)malloc(4096); // Full path to file
    sprintf(path, "%s%s", directory, filename);
    
    FILE* input_file = fopen(path, "r"); // Skip first line
    if (!input_file)
    {    
        fprintf(stderr, "File not found: %s\n", path);
        exit(1);
    }
    char* buffer = (char*)malloc(4096);
    fgets(buffer, 4096, input_file); // Skip first line
    
    int xrel, yrel, speedxrel, speedyrel; // Read input file
    char* color_str = (char*) malloc(4096);
    char* property  = (char*) malloc(4096);
    char* value     = (char*) malloc(4096);
    
    while (fgets(buffer, 4096, input_file))
    {
        buffer += strspn(buffer, " \t"); // Remove whitespace at start
        if (buffer[0] == '%') // Other data (width, height, etc.)
        {
            buffer++;
            char* variable = (char*) malloc(4096);
            sscanf(buffer, "%s %s", property, value);
            str_tolower(property);
            if (property[0] == '.')
            {
                property++;
                sprintf(variable, "%d %s", file_identifier, property);
            }
            else
            {
                strcpy(variable, property);
            }
            set_property(variable, value);
            
            continue;
        }
       
        buffer[strcspn(buffer, "#\0")] = 0;
        
        std::string line(buffer);
        std::istringstream iss(line);
        std::vector<std::string> tokens{std::istream_iterator<std::string>(iss),
                                        std::istream_iterator<std::string>()}; // Split line
                      
        int* int_ptrs[] = {&xrel, &yrel, &speedxrel, &speedyrel, NULL};
        int index = 0;
        for (std::string token: tokens)
        {
            if (int_ptrs[index]) // Set integer values (x pos, y pos, x speed, y speed)
            {
                if (token[0] == '$')
                {
                    if (token[1] == '.') // Local
                    {
                        sprintf(property, "%d %s", file_identifier, token.c_str() + 2); // Create a variable with file-dependent name, e.g. ".foo" -> "1 foo"
                    }
                    else // Global
                    {
                        strcpy(property, token.c_str());
                        property++;
                    }
                    str_tolower(property);
                    *int_ptrs[index] = atoi(get_property(property));
                }
                else
                {
                    if (sscanf(token.c_str(), "%d", int_ptrs[index]) <= 0)
                        continue;
                }
            }
            else // Set color/include
            {
                strcpy(color_str, token.c_str()); 
                if (color_str[0] == '!') // Include
                {
                    color_str++;
                    char* newdir = (char*) malloc(4096);
                    strcpy(newdir, directory);
                    strcat(newdir, getdirname(color_str));
                    read_file(newdir, getfilename(color_str), x + xrel, y + yrel, speedx + speedxrel, speedy + speedyrel);
                }
                else // Color
                {
                    Color color = Colors::read_color(color_str);
                    Qualum::create_qualum(x + xrel, y + yrel, color, speedx + speedxrel, speedy + speedyrel);
                }
            }
            index++;
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
    
    read_file(getdirname(argv[1]), getfilename(argv[1]), 0, 0, 0, 0);
    Qualum::initialize();
    
    window = SDL_CreateWindow("Qualums", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, Rendering::WIDTH, Rendering::HEIGHT,
        SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_Event event;
    bool will_quit = false;
    
    
    
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
                        TIME_SCALE = (double)(event.key.keysym.sym - SDLK_0);
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

