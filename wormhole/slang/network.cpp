#include "network.hpp"

#include <SDL2/SDL_net.h>

using namespace GYDM;

/*************************************************************************************************/
void GYDM::network_initialize() {
    static bool okay = false;
    
    if (!okay) {
        okay = (SDLNet_Init() == 0);

        if (okay) {
            atexit(SDLNet_Quit);
        } else {
            fprintf(stderr, "SDL Net 初始化失败: %s\n", SDLNet_GetError());
        }
    }
}
