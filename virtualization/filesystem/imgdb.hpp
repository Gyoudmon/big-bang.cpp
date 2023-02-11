#pragma once

#include <SDL2/SDL.h>

#include <string>
#include <memory>

namespace WarGrey::STEM {
    class Costume {
    public:
        Costume(SDL_Texture* raw) : costume(raw) {}
        virtual ~Costume() { if (this->okay()) SDL_DestroyTexture(this->costume); }

    public:
        bool okay() { return this->costume != nullptr; }
        SDL_Texture* texture() { return this->costume; }
        void feed_extent(int* width, int* height);
        void feed_extent(float* width, float* height);
        
    private:
        SDL_Texture* costume = nullptr;
    };

    typedef std::shared_ptr<Costume> shared_costume_t;

    /*********************************************************************************************/
    void imgdb_setup(const char* rootdir);
    void imgdb_teardown();

    shared_costume_t imgdb_ref(const char* subpath, SDL_Renderer* rendener);
    shared_costume_t imgdb_ref(const std::string& subpath, SDL_Renderer* rendener);

    void imgdb_remove(const char* subpath);
    void imgdb_remove(const std::string& subpath);

    std::string imgdb_build_path(const std::string& subpath, const std::string& filename, const std::string& extension);
    std::string imgdb_build_path(const char* subpath, const char* filename, const char* extension);

    std::string imgdb_absolute_path(const char* subpath);
    std::string imgdb_absolute_path(const std::string& subpath);
}
