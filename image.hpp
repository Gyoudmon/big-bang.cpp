#pragma once

#include <SDL2/SDL_image.h>

namespace WarGrey::STEM {
    typedef void (*game_lambda_image_f)(SDL_Renderer*, float, float, void*);

    SDL_Surface* game_lambda_image(int width, int height, game_lambda_image_f make_image, void* datum, uint32_t alpha_color_key = 0xFFFFFFU);
    SDL_Surface* game_lambda_image(float width, float height, game_lambda_image_f make_image, void* datum, uint32_t alpha_color_key = 0xFFFFFFU);
    
    SDL_Surface* game_blank_image(int width, int height, uint32_t alpha_color_key = 0xFFFFFFU);
    SDL_Surface* game_blank_image(float width, float height, uint32_t alpha_color_key = 0xFFFFFFU);
    
    SDL_Surface* game_load_image(const char* file);
    SDL_Surface* game_load_image(const std::string& file);
    void game_unload_image(SDL_Surface* image);

    void game_draw_image(SDL_Renderer* renderer, SDL_Surface* image, int x, int y);
    void game_draw_image(SDL_Renderer* renderer, const std::string& file, int x, int y);
    void game_draw_image(SDL_Renderer* renderer, const char* file, int x, int y);
    void game_draw_image(SDL_Renderer* renderer, SDL_Surface* image, int x, int y, int width, int height);
    void game_draw_image(SDL_Renderer* renderer, const std::string& file, int x, int y, int width, int height);
    void game_draw_image(SDL_Renderer* renderer, const char* file, int x, int y, int width, int height);

    void game_draw_image(SDL_Renderer* renderer, SDL_Surface* image, float x, float y);
    void game_draw_image(SDL_Renderer* renderer, const std::string& file, float x, float y);
    void game_draw_image(SDL_Renderer* renderer, const char* file, float x, float y);
    void game_draw_image(SDL_Renderer* renderer, SDL_Surface* image, float x, float y, float width, float height);
    void game_draw_image(SDL_Renderer* renderer, const std::string& file, float x, float y, float width, float height);
    void game_draw_image(SDL_Renderer* renderer, const char* file, float x, float y, float width, float height);
    
    bool game_save_image(SDL_Surface* png, const std::string& pname);
    bool game_save_image(SDL_Surface* png, const char* pname);
}

