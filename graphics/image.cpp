#include <SDL2/SDL.h>                 // 放最前面以兼容 macOS

#include "image.hpp"
#include "geometry.hpp"
#include "colorspace.hpp"

#include "../datum/flonum.hpp"

#include <filesystem>

using namespace WarGrey::STEM;

/*************************************************************************************************/
SDL_Surface* WarGrey::STEM::game_lambda_image(int width, int height, game_lambda_image_f make_image, void* datum, uint32_t alpha_color_key) {
    return game_lambda_image(float(width), float(height), make_image, datum, alpha_color_key);
}

SDL_Surface* WarGrey::STEM::game_lambda_image(float width, float height, game_lambda_image_f make_image, void* datum, uint32_t alpha_color_key) {
    SDL_Surface* surface = game_blank_image(width, height, alpha_color_key);

    if (surface != nullptr) {
        SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);
        
        if (renderer != nullptr) {
            make_image(renderer, width, height, datum);
            SDL_RenderPresent(renderer);
            SDL_DestroyRenderer(renderer);
        }
    }

    return surface;
}

SDL_Surface* WarGrey::STEM::game_blank_image(int width, int height, uint32_t alpha_color_key) {
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    uint8_t r, g, b;

    RGB_FromHexadecimal(alpha_color_key, &r, &g, &b);
    SDL_SetColorKey(surface, 1, SDL_MapRGB(surface->format, r, g, b));

    return surface;
}

SDL_Surface* WarGrey::STEM::game_blank_image(float width, float height, uint32_t alpha_color_key) {
    return game_blank_image(fl2fxi(width), fl2fxi(height), alpha_color_key);
}

SDL_Surface* WarGrey::STEM::game_formatted_image(int width, int height, uint32_t format) {
    return SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, format);
}

SDL_Surface* WarGrey::STEM::game_formatted_image(float width, float height, uint32_t format) {
    return game_formatted_image(fl2fxi(width), fl2fxi(height), format);
}

SDL_Surface* WarGrey::STEM::game_load_image(const std::string& file) {
    return game_load_image(file.c_str());
}

SDL_Surface* WarGrey::STEM::game_load_image(const char* file) {
    return IMG_Load(file);
}

SDL_Texture* WarGrey::STEM::game_load_image_as_texture(SDL_Renderer* renderer, const std::string& file) {
    return game_load_image_as_texture(renderer, file.c_str());
}

SDL_Texture* WarGrey::STEM::game_load_image_as_texture(SDL_Renderer* renderer, const char* file) {
    return IMG_LoadTexture(renderer, file);
}

void WarGrey::STEM::game_unload_image(SDL_Surface* image) {
    SDL_FreeSurface(image);
}

void WarGrey::STEM::game_unload_image(SDL_Texture* image) {
    SDL_DestroyTexture(image);
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, SDL_Surface* image, int x, int y, SDL_RendererFlip flip, double angle) {
    game_render_surface(renderer, image, x, y, flip, angle);
}
    
void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const std::string& file, int x, int y, SDL_RendererFlip flip, double angle) {
    game_draw_image(renderer, file.c_str(), x, y, flip, angle);
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const char* file, int x, int y, SDL_RendererFlip flip, double angle) {
    SDL_Texture* image = game_load_image_as_texture(renderer, file);

    if (image != nullptr) {
        game_render_texture(renderer, image, x, y, flip, angle);
        game_unload_image(image);
    }
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, SDL_Surface* image, int x, int y, int width, int height, SDL_RendererFlip flip, double angle) {
    game_render_surface(renderer, image, x, y, width, height, flip, angle);
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, SDL_Texture* image, int x, int y, int width, int height, SDL_RendererFlip flip, double angle) {
    game_render_texture(renderer, image, x, y, width, height, flip, angle);
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const std::string& file, int x, int y, int width, int height, SDL_RendererFlip flip, double angle) {
    game_draw_image(renderer, file.c_str(), x, y, width, height, flip, angle);
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const char* file, int x, int y, int width, int height, SDL_RendererFlip flip, double angle) {
    SDL_Texture* image = game_load_image_as_texture(renderer, file);

    if (image != nullptr) {
        game_render_texture(renderer, image, x, y, width, height, flip, angle);
        game_unload_image(image);
    }
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, SDL_Surface* image, float x, float y, SDL_RendererFlip flip, double angle) {
    game_render_surface(renderer, image, x, y, flip, angle);
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, SDL_Texture* image, float x, float y, SDL_RendererFlip flip, double angle) {
    game_render_texture(renderer, image, x, y, flip, angle);
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const std::string& file, float x, float y, SDL_RendererFlip flip, double angle) {
    game_draw_image(renderer, file.c_str(), x, y, flip, angle);
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const char* file, float x, float y, SDL_RendererFlip flip, double angle) {
    SDL_Texture* image = game_load_image_as_texture(renderer, file);

    if (image != nullptr) {
        game_render_texture(renderer, image, x, y, flip, angle);
        game_unload_image(image);
    }
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, SDL_Surface* image, float x, float y, float width, float height, SDL_RendererFlip flip, double angle) {
    game_render_surface(renderer, image, x, y, width, height, flip, angle);
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, SDL_Texture* image, float x, float y, float width, float height, SDL_RendererFlip flip, double angle) {
    game_render_texture(renderer, image, x, y, width, height, flip, angle);
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const std::string& file, float x, float y, float width, float height, SDL_RendererFlip flip, double angle) {
    game_draw_image(renderer, file.c_str(), x, y, width, height, flip, angle);
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const char* file, float x, float y, float width, float height, SDL_RendererFlip flip, double angle) {
    SDL_Texture* image = game_load_image_as_texture(renderer, file);

    if (image != nullptr) {
        game_render_texture(renderer, image, x, y, width, height, flip, angle);
        game_unload_image(image);
    }
}

/*************************************************************************************************/
bool WarGrey::STEM::game_save_image(SDL_Surface* png, const std::string& pname) {
    return game_save_image(png, pname.c_str());
}

bool WarGrey::STEM::game_save_image(SDL_Surface* png, const char* pname) {
    bool okay = false;

    if (png != nullptr) {
        create_directories(std::filesystem::path(pname).parent_path());
        if (IMG_SavePNG(png, pname) == 0) {
            okay = true;
        }
    }

    return okay;
}
