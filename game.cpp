#include "game.hpp"          // 放最前面以兼容 macOS
#include "vsntext.hpp"
#include "colorspace.hpp"
#include "text.hpp"
#include "time.hpp"

#include <iostream>
#include <unordered_map>
#include <filesystem>

using namespace WarGrey::STEM;
using namespace std::filesystem;

/*************************************************************************************************/
#define Call_With_Error_Message(init, message, GetError) \
    if (init != 0) { \
        std::cout << message << std::string(GetError()) << std::endl; \
        exit(1); \
    }

#define Call_With_Safe_Exit(init, message, quit, GetError) \
    Call_With_Error_Message(init, message, GetError); \
    atexit(quit);

#define Call_For_Variable(id, init, failure, message, GetError) \
    id = init; \
    if (id == failure) { \
        std::cout << message << std::string(GetError()) << std::endl; \
        exit(1); \
    }

#define Game_Close_Font(id) if (id != NULL) TTF_CloseFont(id); id = NULL

/*************************************************************************************************/
TTF_Font* WarGrey::STEM::GAME_DEFAULT_FONT = NULL;
TTF_Font* WarGrey::STEM::game_sans_serif_font = NULL;
TTF_Font* WarGrey::STEM::game_serif_font = NULL;
TTF_Font* WarGrey::STEM::game_monospace_font = NULL;
TTF_Font* WarGrey::STEM::game_math_font = NULL;
TTF_Font* WarGrey::STEM::game_unicode_font = NULL;

/*************************************************************************************************/
static std::unordered_map<std::string, std::string> system_fonts;
static std::string system_fontdirs[] = {
    "/System/Library/Fonts",
    "/Library/Fonts",
    "C:\\Windows\\Fonts",
    "/usr/share/fonts"
};

static void game_push_fonts_of_directory(path& root) {
    for (auto entry : directory_iterator(root)) {
        path self = entry.path();

        if (entry.is_directory()) {
            game_push_fonts_of_directory(self);
        } else if (entry.is_regular_file()) {
            system_fonts[self.filename().string()] = self.string();
        }
    }
}

static void game_fonts_initialize(int fontsize = 16) {
    for (unsigned int idx = 0; idx < sizeof(system_fontdirs) / sizeof(std::string); idx++) {
        path root(system_fontdirs[idx]);

        if (exists(root) && is_directory(root)) {
            game_push_fonts_of_directory(root);
        }
    }

#if defined(__macosx__)
    game_sans_serif_font = game_create_font("LucidaGrande.ttc", fontsize);
    game_serif_font = game_create_font("Times.ttc", fontsize);
    game_monospace_font = game_create_font("Courier.ttc", fontsize);
    game_math_font = game_create_font("Bodoni 72.ttc", fontsize);
    game_unicode_font = game_create_font("PingFang.ttc", fontsize);
#elif defined(__windows__) /* HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\Fonts */
    game_sans_serif_font = game_create_font("msyh.ttc", fontsize); // Microsoft YaHei
    game_serif_font = game_create_font("times.ttf", fontsize); // Times New Roman
    game_monospace_font = game_create_font("cour.ttf", fontsize); // Courier New
    game_math_font = game_create_font("BOD_R.TTF", fontsize); // Bodoni MT
    game_unicode_font = game_create_font("msyh.ttc", fontsize);
#else /* the following fonts have not been tested */
    game_sans_serif_font = game_create_font("Nimbus Sans.ttc", fontsize);
    game_serif_font = game_create_font("DejaVu Serif.ttc", fontsize);
    game_monospace_font = game_create_font("Monospace.ttf", fontsize);
    game_math_font = game_create_font("URW Bookman.ttf", fontsize);
    game_unicode_font = game_create_font("Arial Unicode.ttf", fontsize);
#endif

    GAME_DEFAULT_FONT = game_sans_serif_font;
}

static void game_fonts_destroy() {
    Game_Close_Font(GAME_DEFAULT_FONT);
}

/*************************************************************************************************/
typedef struct timer_parcel {
    Universe* universe;
    uint32_t interval;
    uint32_t count;
    uint32_t uptime;
} timer_parcel_t;

/**
 * 本函数在定时器到期时执行, 并将该事件报告给事件系统，以便绘制下一帧动画
 * @param interval, 定时器等待时长，以 ms 为单位
 * @param datum,    用户数据，传递给 SDL_AddTimer 的第三个参数会出现在这
 * @return 返回定时器下次等待时间。注意定时器的实际等待时间是该返回值减去执行该函数所花时间
 **/
static unsigned int trigger_timer_event(unsigned int interval, void* datum) {
    timer_parcel_t* parcel = reinterpret_cast<timer_parcel_t*>(datum);
    SDL_UserEvent user_event;
    SDL_Event timer_event;

    user_event.type = SDL_USEREVENT;
    user_event.code = 0;

    user_event.data1 = datum;

    parcel->count += 1;
    parcel->interval = interval;
    parcel->uptime = SDL_GetTicks();

    // 将该事件报告给事件系统
    timer_event.type = user_event.type;
    timer_event.user = user_event;
    SDL_PushEvent(&timer_event);

    return interval;
}

/*************************************************************************************************/
static void game_initialize(uint32_t flags, int fontsize = 16) {
    if (GAME_DEFAULT_FONT == NULL) {
        Call_With_Safe_Exit(SDL_Init(flags), "SDL 初始化失败: ", SDL_Quit, SDL_GetError);
        Call_With_Safe_Exit(TTF_Init(), "TTF 初始化失败: ", TTF_Quit, TTF_GetError);

#if defined(__macosx__)
        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
#else
        IMG_Init(IMG_INIT_PNG);
#endif
        /* manually check errors */ {
            std::string err = std::string(IMG_GetError());

            if (err.size() > 0) {
                std::cout << "IMG 初始化失败: " << err << std::endl;
                exit(1);
            }
        }
    
        // SDL_SetHint("SDL_IME_SHOW_UI", "1");

        game_fonts_initialize(fontsize);

        atexit(IMG_Quit);
        atexit(game_fonts_destroy);
    }
}

static SDL_Texture* game_create_texture(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_Texture* texture;
    int width, height;

    SDL_GetWindowSize(window, &width, &height);
    Call_For_Variable(texture,
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height),
        NULL, "纹理创建失败: ", SDL_GetError);

    return texture;
}

static SDL_Texture* game_create_world(int width, int height, SDL_Window** window, SDL_Renderer** renderer) {
    uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

    if ((width <= 0) || (height <= 0)) {
        if ((width <= 0) && (height <= 0)) {
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        } else {
            flags |= SDL_WINDOW_MAXIMIZED;
        }
    }

    Call_With_Error_Message(SDL_CreateWindowAndRenderer(width, height, flags, window, renderer),
        "SDL 窗体和渲染器创建失败: ", SDL_GetError);

    return game_create_texture((*window), (*renderer));
}

static inline void game_world_reset(SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc) {
    unsigned char r, g, b;

    RGB_FromHexadecimal(bgc, &r, &g, &b);
    SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
    SDL_RenderClear(renderer);

    RGB_FromHexadecimal(fgc, &r, &g, &b);
    SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
}

static inline void game_world_reset(SDL_Renderer* renderer, SDL_Texture* texture, uint32_t fgc, uint32_t bgc) {
    SDL_SetRenderTarget(renderer, texture);
    game_world_reset(renderer, fgc, bgc);
}

static inline void game_world_refresh(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_SetRenderTarget(renderer, texture);
}

/*************************************************************************************************/
TTF_Font* WarGrey::STEM::game_create_font(const char* face, int fontsize) {
    std::string face_key(face);
    TTF_Font* font = NULL;

    if (system_fonts.find(face_key) == system_fonts.end()) {
        font = TTF_OpenFont(face, fontsize);
    } else {
        font = TTF_OpenFont(system_fonts[face_key].c_str(), fontsize);
    }

    if (font == NULL) {
        fprintf(stderr, "无法加载字体 '%s': %s\n", face, TTF_GetError());
    }

    return font;
}

void WarGrey::STEM::game_font_destroy(TTF_Font* font) {
    TTF_CloseFont(font);
}

const std::string* WarGrey::STEM::game_font_list(int* n, int fontsize) {
    static std::string* font_list = new std::string[system_fonts.size()];
    static int i = 0;

    if (i == 0) {
        for (std::pair<std::string, std::string> k_v : system_fonts) {
            TTF_Font* f = TTF_OpenFont(k_v.second.c_str(), fontsize);

            if (f != NULL) {
                font_list[i ++] = k_v.first;
                
                // insufficient resources to open all fonts
                TTF_CloseFont(f);
            }
        }
    }
    
    if (n != NULL) {
        (*n) = i;
    }

    return (const std::string*)font_list;
}

/*************************************************************************************************/
WarGrey::STEM::Universe::Universe() : Universe("Big Bang!") {}

WarGrey::STEM::Universe::Universe(const char *title, int fps, uint32_t fgc, uint32_t bgc)
    : _fps(fps), _fgc(fgc), _bgc(bgc), _mfgc(fgc), in_editing(false), current_usrin(NULL), echo_font(NULL)
      , update_sequence_depth(0), update_is_needed(false) {
    
    // 初始化游戏系统
    game_initialize(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    
    // 创建游戏世界
    this->texture = game_create_world(1, 0, &this->window, &this->renderer);

    // 设置标题
    SDL_SetWindowTitle(this->window, title);

    // 按指定颜色重置窗口
    game_world_reset(this->renderer, this->texture, this->_fgc, this->_bgc);

    this->set_blend_mode(SDL_BLENDMODE_NONE);
}

WarGrey::STEM::Universe::~Universe() {
    if (this->timer > 0) {
        SDL_RemoveTimer(this->timer);
    }

    SDL_DestroyTexture(this->texture);
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
}

void WarGrey::STEM::Universe::big_bang() {
    uint32_t quit_time = 0UL;           // 游戏退出时的在线时间
    SDL_Event e;                        // SDL 事件
    timer_parcel_t parcel;
    
    if (this->_fps > 0) {
        parcel.universe = this;
        parcel.interval = 1000 / this->_fps;
        parcel.count = 0;
        parcel.uptime = 0;

        this->timer = Call_For_Variable(timer,
                SDL_AddTimer(parcel.interval, trigger_timer_event, reinterpret_cast<void*>(&parcel)),
                0, "定时器创建失败: ", SDL_GetError);
    }

    this->fill_window_size(&this->window_width, &this->window_height);
    SDL_SetRenderTarget(this->renderer, this->texture);
    this->begin_update_sequence();
    this->reflow(this->window_width, this->window_height);
    this->notify_updated();
    this->end_update_sequence();

    /* 游戏主循环 */
    while ((quit_time == 0UL) && !this->can_exit()) {
        if (SDL_WaitEvent(&e)) {        // 处理用户交互事件, SDL_PollEvent 多占用 4-7% CPU
            this->begin_update_sequence();

            switch (e.type) {
            case SDL_USEREVENT: {       // 定时器到期通知，更新游戏
                auto parcel = reinterpret_cast<timer_parcel_t*>(e.user.data1);

                if (parcel->universe == this) {
                    this->on_elapse(parcel->interval, parcel->count, parcel->uptime);
                }
            }; break;
            case SDL_MOUSEMOTION: this->on_mouse_event(e.motion); break;
            case SDL_MOUSEWHEEL: this->on_mouse_event(e.wheel); break;
            case SDL_MOUSEBUTTONUP: this->on_mouse_event(e.button, false); break;
            case SDL_MOUSEBUTTONDOWN: this->on_mouse_event(e.button, true);  break;
            case SDL_KEYUP: this->on_keyboard_event(e.key, false); break;
            case SDL_KEYDOWN: this->on_keyboard_event(e.key, true); break;
            case SDL_TEXTINPUT: this->on_user_input(e.text.text); break;
            case SDL_TEXTEDITING: this->on_editing(e.edit.text, e.edit.start, e.edit.length); break;
            case SDL_WINDOWEVENT: {
                switch (e.window.event) {
                    case SDL_WINDOWEVENT_RESIZED: this->on_resize(e.window.data1, e.window.data2); break;
                }
            }; break;
            case SDL_QUIT: {
                if (this->timer > 0UL) {
                    SDL_RemoveTimer(this->timer); // 停止定时器
                    this->timer = 0;
                }

                quit_time = e.quit.timestamp;
            }; break;
            default: /* std::cout << "Ignored unhandled event(type = " << e.type << ")" << std::endl; */ break;
            }
   
            this->end_update_sequence();
        }
    }
}

void WarGrey::STEM::Universe::on_elapse(uint32_t interval, uint32_t count, uint32_t uptime) {
    this->update(interval, count, uptime);
    this->notify_updated();
}

void WarGrey::STEM::Universe::on_mouse_event(SDL_MouseButtonEvent &mouse, bool pressed) {
    if (!pressed) {
        if (mouse.clicks == 1) {
            switch (mouse.button) {
                case SDL_BUTTON_LEFT: this->on_click(mouse.x, mouse.y); break;
                case SDL_BUTTON_RIGHT: this->on_right_click(mouse.x, mouse.y); break;
            }
        } else {
            switch (mouse.button) {
                case SDL_BUTTON_LEFT: this->on_double_click(mouse.x, mouse.y); break;
            }
        }
    }
}

void WarGrey::STEM::Universe::on_mouse_event(SDL_MouseMotionEvent &mouse) {
    this->on_mouse_move(mouse.state, mouse.x, mouse.y, mouse.xrel, mouse.yrel);
}

void WarGrey::STEM::Universe::on_mouse_event(SDL_MouseWheelEvent &mouse) {
    int horizon = mouse.x;
    int vertical = mouse.y;
    float hprecise = float(horizon);  // mouse.preciseX;
    float vprecise = float(vertical); // mouse.preciseY;

    if (mouse.direction == SDL_MOUSEWHEEL_FLIPPED) {
        horizon  *= -1;
        vertical *= -1;
        hprecise *= -1.0F;
        vprecise *= -1.0F;
    }

    this->on_scroll(horizon, vertical, hprecise, vprecise);
}

void WarGrey::STEM::Universe::on_keyboard_event(SDL_KeyboardEvent &keyboard, bool pressed) {
    SDL_Keysym key = keyboard.keysym;

    if (this->in_editing) {
        if (!pressed) {
            switch (key.sym) {
                case SDLK_RETURN: this->enter_input_text(); break;
                case SDLK_BACKSPACE: this->popback_input_text(); break;
            }
        }
    } else {
        if (!pressed) {
            int ctrl_mod = KMOD_LCTRL | KMOD_RCTRL;

#if defined(__macosx__)
            ctrl_mod = ctrl_mod | KMOD_LGUI | KMOD_RGUI;
#endif

            if (key.mod & ctrl_mod) {
                switch (key.sym) {
                    case SDLK_s: this->save(); break;
                    case SDLK_p: this->take_snapshot(); break;
                    default: this->on_char(key.sym, key.mod, keyboard.repeat, pressed);
                }
            } else {
                this->on_char(key.sym, key.mod, keyboard.repeat, pressed);
            }
        } else {
            this->on_char(key.sym, key.mod, keyboard.repeat, pressed);
        }
    }
}

void WarGrey::STEM::Universe::on_resize(int width, int height) {
    this->window_width = width;
    this->window_height = height;

    SDL_DestroyTexture(this->texture);
    this->texture = game_create_texture(this->window, this->renderer);
    
    this->begin_update_sequence();
    game_world_reset(this->renderer, this->texture, this->_fgc, this->_bgc);
    this->reflow(width, height);
    this->notify_updated();
    this->end_update_sequence();
}

void WarGrey::STEM::Universe::on_user_input(const char* text) {
    if (this->in_editing) {
        this->usrin.append(text);
        this->current_usrin = NULL;
        
        if (this->display_usr_input_and_caret(this->renderer, true)) {
            this->notify_updated();
        }
    }

    this->on_text(text, false);
}

void WarGrey::STEM::Universe::on_editing(const char* text, int pos, int span) {
    this->current_usrin = text;
    this->on_text(text, pos, span);
}

void WarGrey::STEM::Universe::do_redraw(SDL_Renderer* renderer, int x, int y, int width, int height) {
    game_world_reset(renderer, this->_fgc, this->_bgc);
    
    if (this->in_editing) {
        this->display_usr_input_and_caret(renderer, true);
    } else {
        this->display_usr_message();
    }

    this->draw(renderer, x, y, width, height);
}

bool WarGrey::STEM::Universe::display_usr_message() {
    bool updated = false;

    if ((this->echo.w > 0) && (this->echo.h > 0)) {
        game_fill_rect(this->renderer, &this->echo, this->_ibgc, 0xFF);

        if (!this->message.empty()) {
            game_draw_blended_text(this->echo_font, renderer, this->_mfgc,
                    this->echo.x, this->echo.y, this->message, this->echo.w);
        }

        updated = true;
    } else {
        if (this->needs_termio_if_no_echo && !this->message.empty()) {
            std::cout << this->message << std::endl;
            this->needs_termio_if_no_echo = false;
        }
    }

    return updated;
}

bool WarGrey::STEM::Universe::display_usr_input_and_caret(SDL_Renderer* renderer, bool yes) {
    bool updated = false;

    if ((this->echo.w > 0) && (this->echo.h > 0)) {
        game_fill_rect(this->renderer, &this->echo, this->_ibgc, 0xFF);

        if (yes) {
            if (this->prompt.empty()) {
                game_draw_blended_text(this->echo_font, renderer, this->_ifgc,
                        this->echo.x, this->echo.y, this->usrin + "_", this->echo.w);
            } else {
                game_draw_blended_text(this->echo_font, renderer, this->_ifgc,
                        this->echo.x, this->echo.y, this->prompt + this->usrin + "_", this->echo.w);
            }
        }

        updated = true;
    }

    return updated;
}

/*************************************************************************************************/
void WarGrey::STEM::Universe::set_blend_mode(SDL_BlendMode bmode) {
    SDL_SetRenderDrawBlendMode(this->renderer, bmode);
}

void WarGrey::STEM::Universe::set_window_title(std::string& title) {
    SDL_SetWindowTitle(this->window, title.c_str());
}

void WarGrey::STEM::Universe::set_window_title(const char* fmt, ...) {
    VSNPRINT(title, fmt);
    this->set_window_title(title);
}

void WarGrey::STEM::Universe::set_window_size(int width, int height, bool centerize) {
    SDL_SetWindowSize(this->window, width, height);

    if (centerize) {
        SDL_SetWindowPosition(this->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    this->on_resize(width, height);
}

void WarGrey::STEM::Universe::fill_window_size(int* width, int* height) {
    SDL_GetWindowSize(this->window, width, height);
}

void WarGrey::STEM::Universe::set_window_fullscreen(bool yes) {
    if (yes) {
        SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(this->window, 0);
    }
}

void WarGrey::STEM::Universe::refresh() {
    this->do_redraw(this->renderer, 0, 0, this->window_width, this->window_height);
    game_world_refresh(this->renderer, this->texture);
}

/*************************************************************************************************/
void WarGrey::STEM::Universe::set_input_echo_area(int x, int y, int width, int height, int fgc, int bgc, TTF_Font* font) {
    this->echo.x = x;
    this->echo.y = y;
    this->echo.w = width;
    this->echo.h = height;

    this->_ifgc = (fgc < 0) ? this->_fgc : fgc;
    this->_ibgc = (bgc < 0) ? this->_bgc : bgc;
    this->echo_font = font;

    SDL_SetTextInputRect(&this->echo);
}

void WarGrey::STEM::Universe::send_message(const char* fmt, ...) {
    VSNPRINT(text, fmt);
    this->send_message(this->_mfgc, text);
}

void WarGrey::STEM::Universe::send_message(uint32_t fgc, const char* fmt, ...) {
    VSNPRINT(text, fmt);
    this->send_message(fgc, text);
}

void WarGrey::STEM::Universe::send_message(uint32_t fgc, const std::string& msg) {
    this->needs_termio_if_no_echo = true;
    this->message = msg;
    this->_mfgc = fgc;

    if (this->display_usr_message()){
        this->notify_updated();
    }
}

void WarGrey::STEM::Universe::start_input_text(const char* fmt, ...) {
    VSNPRINT(p, fmt);
    this->start_input_text(p);
}

void WarGrey::STEM::Universe::start_input_text(const std::string& p) {
    if (p.size() > 0) {
        this->prompt = p;
    }

    SDL_StartTextInput();
    this->in_editing = true;
    this->message.erase();
    
    if (this->display_usr_input_and_caret(this->renderer, true)) {
        this->notify_updated();
    }
}

void WarGrey::STEM::Universe::stop_input_text() {
    this->in_editing = false;
    SDL_StopTextInput();
    this->on_text(this->usrin.c_str(), true);
    this->usrin.erase();
    this->prompt.erase();

    if (this->display_usr_input_and_caret(this->renderer, false)) {
        this->notify_updated();
    }
}

void WarGrey::STEM::Universe::enter_input_text() {
    if (this->current_usrin != NULL) {
        this->on_user_input(this->current_usrin);
        this->current_usrin = NULL;
    } else {
        this->stop_input_text();
    }
}

void WarGrey::STEM::Universe::popback_input_text() {
    size_t size = this->usrin.size();

    if (size > 0) {
        const unsigned char* text = reinterpret_cast<const unsigned char*>(this->usrin.c_str());
        
        /**
         * UTF-8 encodes characters in 1 to 4 bytes, and their binary forms are:
         *   0xxx xxxx
         *   110x xxxx  10xx xxxx
         *   1110 xxxx  10xx xxxx  10xx xxxx
         *   1111 xxxx  10xx xxxx  10xx xxxx  10xx xxxx
         */
        
        if (text[size - 1] < 0b10000000U) {
            this->usrin.pop_back();
        } else {
            size -= 2;
            while (text[size] < 0b11000000U) size--;
            this->usrin.erase(size);
        }

        if (this->display_usr_input_and_caret(this->renderer, true)) {
            this->notify_updated();
        }
    }
}

/*************************************************************************************************/
bool WarGrey::STEM::Universe::snapshot(const std::string& path) {
    return this->snapshot(path.c_str());
}

bool WarGrey::STEM::Universe::snapshot(const char* pname) {
    SDL_Renderer* renderer = NULL;
    SDL_Surface* snapshot = NULL;
    uint32_t saved_fgc = this->_mfgc;
    bool okay = false;

    snapshot = SDL_CreateRGBSurface(0, this->window_width, this->window_height, 32, 0, 0, 0, 0);

    if (snapshot != NULL) {
        renderer = SDL_CreateSoftwareRenderer(snapshot);
        
        if (renderer != NULL) {
            this->do_redraw(renderer, 0, 0, this->window_width, this->window_height);

            create_directories(path(pname).parent_path());
            if (IMG_SavePNG(snapshot, pname) == 0) {
                okay = true;
            } else {
                this->send_message(0xFF0000, "failed to save snapshot: %s", SDL_GetError());
            }
        } else {
            this->send_message(0xFF0000, "failed to take snapshot: %s", SDL_GetError());
        }
    } else {
         this->send_message(0xFF0000, "failed to take snapshot: %s", SDL_GetError());
    }

    if (snapshot != NULL) {
        SDL_FreeSurface(snapshot);
    }

    if (renderer != NULL) {
        SDL_DestroyRenderer(renderer);
    }

    if (!okay) {
        this->_mfgc = saved_fgc;
    }

    return okay;
}

void WarGrey::STEM::Universe::take_snapshot() {
    const char* basename = SDL_GetWindowTitle(this->window);
    long long ms = current_milliseconds();
    long long s = ms / 1000;
    path snapshot_png = (this->snapshot_rootdir.empty() ? current_path() : path(this->snapshot_rootdir))
        / path(game_create_string("%s-%s.%lld.png", basename, make_timestamp_utc(s, true).c_str(), ms % 1000));

    if (this->snapshot(snapshot_png.string().c_str())) { // stupid windows as it requires `string()`
        this->send_message("A snapshot has been saved as '%s'.", snapshot_png.c_str());
    }
}

void WarGrey::STEM::Universe::set_snapshot_folder(const char* dir) {
    this->set_snapshot_folder(std::string(dir));
}

void WarGrey::STEM::Universe::set_snapshot_folder(const std::string& dir) {
    this->snapshot_rootdir = path(dir).make_preferred().string();
}

/*************************************************************************************************/
void WarGrey::STEM::Universe::notify_updated() {
    if (this->in_update_sequence()) {
        this->update_is_needed = true;
    } else {
        this->refresh();
        this->update_is_needed = false;
    }
}

void WarGrey::STEM::Universe::end_update_sequence() {
    this->update_sequence_depth -= 1;

    if (this->update_sequence_depth < 1) {
        this->update_sequence_depth = 0;

        if (this->update_is_needed) {
            this->refresh();
            this->update_is_needed = false;
        }
    }

}

