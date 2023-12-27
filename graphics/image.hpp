#pragma once

#include <SDL2/SDL_image.h>

#include <cstdint>
#include <string>

namespace GYDM {
    __lambda__ SDL_Texture* game_blank_image(SDL_Renderer* renderer, int width, int height);
    __lambda__ SDL_Texture* game_blank_image(SDL_Renderer* renderer, float width, float height);

    __lambda__ SDL_Surface* game_formatted_surface(int width, int height, uint32_t format);
    __lambda__ SDL_Surface* game_formatted_surface(float width, float height, uint32_t format);
    
    __lambda__ SDL_Texture* game_load_image(SDL_Renderer* renderer, const char* file);
    __lambda__ SDL_Texture* game_load_image(SDL_Renderer* renderer, const std::string& file);
    __lambda__ void game_clear_image(SDL_Renderer* renderer, SDL_Texture* image);
    __lambda__ void game_unload_image(SDL_Texture* image);

    __lambda__ bool game_save_image(SDL_Surface* png, const std::string& pname);
    __lambda__ bool game_save_image(SDL_Surface* png, const char* pname);
}
