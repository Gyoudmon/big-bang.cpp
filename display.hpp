#ifndef _WARGREY_STEM_IDISPLAY_H
#define _WARGREY_STEM_IDISPLAY_H

#include <SDL2/SDL.h>

#include <string>

/*************************************************************************************************/
namespace WarGrey::STEM {
    class IDisplay {
        public:
            IDisplay() {}
            virtual ~IDisplay() {}

        public:
            virtual void refresh() = 0;
            virtual SDL_Surface* snapshot() = 0;

        public:
            bool save_snapshot(const std::string& path);
            bool save_snapshot(const char* path);
    };
}

#endif

