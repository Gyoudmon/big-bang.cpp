#include "game.hpp"                 // 放最前面以兼容 macOS
#include "image.hpp"
#include "colorspace.hpp"

#include "datum/flonum.hpp"

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

SDL_Surface* WarGrey::STEM::game_load_image(const std::string& file) {
    return game_load_image(file.c_str());
}

SDL_Surface* WarGrey::STEM::game_load_image(const char* file) {
    return IMG_Load(file);
}

void WarGrey::STEM::game_unload_image(SDL_Surface* image) {
    SDL_FreeSurface(image);
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, SDL_Surface* image, int x, int y) {
    game_render_surface(renderer, image, x, y);
}
    
void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const std::string& file, int x, int y) {
    game_draw_image(renderer, file.c_str(), x, y);
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const char* file, int x, int y) {
    SDL_Surface* image = game_load_image(file);

    if (image != nullptr) {
        game_draw_image(renderer, image, x, y);
        game_unload_image(image);
    }
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, SDL_Surface* image, int x, int y, int width, int height) {
    if ((image->w == width) && (image->h == height)) {
        game_render_surface(renderer, image, x, y);
    } else {
        SDL_Rect region;

        region.x = x;
        region.y = y;
        region.w = (width <= 0) ? image->w : width;
        region.h = (height <= 0) ? image->h : height;
        game_render_surface(renderer, image, &region);
    }
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const std::string& file, int x, int y, int width, int height) {
    game_draw_image(renderer, file.c_str(), x, y, width, height);
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const char* file, int x, int y, int width, int height) {
    SDL_Surface* image = game_load_image(file);

    if (image != nullptr) {
        game_draw_image(renderer, image, x, y, width, height);
        game_unload_image(image);
    }
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, SDL_Surface* image, float x, float y) {
    game_render_surface(renderer, image, fl2fxi(x), fl2fxi(y));
}
    
void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const std::string& file, float x, float y) {
    game_draw_image(renderer, file.c_str(), fl2fxi(x), fl2fxi(y));
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const char* file, float x, float y) {
    game_draw_image(renderer, file, fl2fxi(x), fl2fxi(y));
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, SDL_Surface* image, float x, float y, float width, float height) {
    game_draw_image(renderer, image, fl2fxi(x), fl2fxi(y), fl2fxi(width), fl2fxi(height));
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const std::string& file, float x, float y, float width, float height) {
    game_draw_image(renderer, file.c_str(), fl2fxi(x), fl2fxi(y), fl2fxi(width), fl2fxi(height));
}

void WarGrey::STEM::game_draw_image(SDL_Renderer* renderer, const char* file, float x, float y, float width, float height) {
    game_draw_image(renderer, file, fl2fxi(x), fl2fxi(y), fl2fxi(width), fl2fxi(height));
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

