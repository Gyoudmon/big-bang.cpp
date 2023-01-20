#pragma once

#include <SDL2/SDL.h>

#include "graphics/colorspace.hpp"
#include "graphics/named_colors.hpp"
#include "graphics/geometry.hpp"
#include "graphics/font.hpp"
#include "graphics/text.hpp"

#include "world.hpp"
#include "matter/graphlet.hpp"
#include "matter/graphlet/textlet.hpp"
#include "matter/graphlet/dimensionlet.hpp"
#include "matter/sprite.hpp"
#include "matter/sprite/folder.hpp"
#include "matter/sprite/sheet.hpp"

#include "virtualization/filesystem/imgdb.hpp"

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
