#pragma once

#include <SDL2/SDL.h>

#include "named_colors.hpp"
#include "geometry.hpp"
#include "universe.hpp"
#include "font.hpp"

#include "graphlet/graphlet.hpp"

/**
 * 为兼容 Windows 而变得丑陋
 *
 * vcpkg 不直接提供 SDL2main
 * SDL2.h 把 main 改名为 SDL_main 了
 * 导致 MSVC 连接器找不到程序入口
 *
 * 手动链接会引发别的问题，比如跟默认库冲突
 * (否则 vcpkg 作者肯定脑子有坑才会整出“手动链接”这种脱了裤子那啥的活)
 */
#ifdef main
#undef main
#endif

