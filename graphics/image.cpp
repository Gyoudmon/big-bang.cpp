#include <SDL2/SDL.h>                 // 放最前面以兼容 macOS
#include <filesystem>

#include "image.hpp"
#include "../datum/flonum.hpp"

using namespace GYDM;

/*************************************************************************************************/
SDL_Texture* GYDM::game_blank_image(SDL_Renderer* renderer, int width, int height) {
    SDL_Texture* image = SDL_CreateTexture(renderer,
                            SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                            width, height);

    if (image != nullptr) {
        SDL_Texture* origin = SDL_GetRenderTarget(renderer);

        // Enable the alpha channel
        SDL_SetTextureBlendMode(image, SDL_BLENDMODE_BLEND);

        SDL_SetRenderTarget(renderer, image);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, origin);
    }
    
    return image;
}

SDL_Texture* GYDM::game_blank_image(SDL_Renderer* renderer, float width, float height) {
    return game_blank_image(renderer, fl2fxi(width), fl2fxi(height));
}

SDL_Surface* GYDM::game_formatted_surface(int width, int height, uint32_t format) {
    return SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, format);
}

SDL_Surface* GYDM::game_formatted_surface(float width, float height, uint32_t format) {
    return game_formatted_surface(fl2fxi(width), fl2fxi(height), format);
}

SDL_Texture* GYDM::game_load_image(SDL_Renderer* renderer, const std::string& file) {
    return game_load_image(renderer, file.c_str());
}

SDL_Texture* GYDM::game_load_image(SDL_Renderer* renderer, const char* file) {
    return IMG_LoadTexture(renderer, file);
}

void GYDM::game_clear_image(SDL_Renderer* renderer, SDL_Texture* image) {
    SDL_Texture* origin = SDL_GetRenderTarget(renderer);
            
    SDL_SetRenderTarget(renderer, image);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, origin);
}

void GYDM::game_unload_image(SDL_Texture* image) {
    SDL_DestroyTexture(image);
}

/*************************************************************************************************/
bool GYDM::game_save_image(SDL_Surface* png, const std::string& pname) {
    return game_save_image(png, pname.c_str());
}

bool GYDM::game_save_image(SDL_Surface* png, const char* pname) {
    bool okay = false;

    if (png != nullptr) {
        create_directories(std::filesystem::path(pname).parent_path());
        if (IMG_SavePNG(png, pname) == 0) {
            okay = true;
        }
    }

    return okay;
}
