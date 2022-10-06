#include "game.hpp"                 // 放最前面以兼容 macOS
#include "image.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
void safe_render_text_surface(SDL_Renderer* target, SDL_Surface* message, SDL_Rect* region) {
    /** TODO: Cache the textures of text **/
    
    SDL_Texture* text = SDL_CreateTextureFromSurface(target, message);

    if (text != NULL) {
        SDL_RenderCopy(target, text, NULL, region);
        SDL_DestroyTexture(text);
    }

    SDL_FreeSurface(message);
}

/*************************************************************************************************/
SDL_Surface* WarGrey::STEM::game_load_image(const std::string& file) {
    return game_load_image(file.c_str());
}

SDL_Surface* WarGrey::STEM::game_load_image(const char* file) {
    SDL_Surface* surface = IMG_Load(file);

    if (surface == NULL) {
        fprintf(stderr, "无法加载图片: %s\n", TTF_GetError());
    }

    return surface;
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

    if (image != NULL) {
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

    if (image != NULL) {
        game_draw_image(renderer, image, x, y, width, height);
        game_unload_image(image);
    }
}

