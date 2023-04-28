#include "universe.hpp"
#include "misc.hpp"

#include "graphics/geometry.hpp"
#include "graphics/colorspace.hpp"
#include "graphics/text.hpp"
#include "graphics/image.hpp"

#include "datum/string.hpp"
#include "datum/box.hpp"
#include "datum/time.hpp"

#include <filesystem>

using namespace WarGrey::STEM;
using namespace std::filesystem;

/*************************************************************************************************/
#define Call_With_Error_Message(init, message, GetError) \
    if (init != 0) { \
        fprintf(stderr, "%s%s\n", message, GetError()); \
        exit(1); \
    }

#define Call_With_Safe_Exit(init, message, quit, GetError) \
    Call_With_Error_Message(init, message, GetError); \
    atexit(quit);

#define Call_For_Variable(id, init, failure, message, GetError) \
    id = init; \
    if (id == failure) { \
        fprintf(stderr, "%s%s\n", message, GetError()); \
        exit(1); \
    }

/*************************************************************************************************/
typedef struct timer_parcel {
    IUniverse* universe;
    uint32_t interval;
    uint64_t count = 0;
    uint64_t uptime = 0;
    uint64_t last_timestamp = 0;
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

    parcel->count += 1U;
    parcel->interval = interval;
    parcel->uptime = SDL_GetTicks();

    // 将该事件报告给事件系统
    timer_event.type = user_event.type;
    timer_event.user = user_event;
    SDL_PushEvent(&timer_event);

    return interval;
}

/*************************************************************************************************/
static void game_initialize(uint32_t flags) {
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
            fprintf(stderr, "IMG 初始化失败: %s\n", err.c_str());
            exit(1);
        }
    }
        
    // SDL_SetHint("SDL_IME_SHOW_UI", "1");

    game_fonts_initialize();

    atexit(IMG_Quit);
    atexit(game_fonts_destroy);
}

static SDL_Texture* game_create_texture(SDL_Window* window, SDL_Renderer* renderer, float* backing_scale = nullptr) {
    SDL_Texture* texture;
    int r_width, r_height;

    SDL_GetRendererOutputSize(renderer, &r_width, &r_height);

    if (backing_scale != nullptr) {
        int win_width, win_height;
    
        SDL_GetWindowSize(window, &win_width, &win_height);
        (*backing_scale) = float(r_width) / float(win_width);
    }

    Call_For_Variable(texture,
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, r_width, r_height),
        nullptr, "纹理创建失败: ", SDL_GetError);


    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    return texture;
}

static void game_create_world(int width, int height, SDL_Window** window, SDL_Renderer** renderer) {
    /**
     * WARNING
     *   Don't use `SDL_WINDOW_ALLOW_HIGHDPI` as it makes everything aliasing.
     *   By default, SDL handles the scaling automatically.
     *   With `SDL_WINDOW_ALLOW_HIGHDPI` you have to handle the scaling on your own.
     *
     * TODO:
     *   Without `SDL_WINDOW_ALLOW_HIGHDPI`,
     *   onscreen text looks either aliasing or blur
     *   as TTF_Font has its own strategy of antialiasing...
     */

    uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    if ((width <= 0) || (height <= 0)) {
        if ((width <= 0) && (height <= 0))  {
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        } else {
            flags |= SDL_WINDOW_MAXIMIZED;
        }
    }

    Call_With_Error_Message(SDL_CreateWindowAndRenderer(width, height, flags, window, renderer),
        "SDL 窗体和渲染器创建失败: ", SDL_GetError);
}

static inline void game_world_reset(SDL_Renderer* renderer, uint32_t fgc, uint32_t bgc) {
    unsigned char r, g, b;
    RGB_From_Hexadecimal(bgc, &r, &g, &b);
    SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF); // the `alpha` does not affect the window
    SDL_RenderClear(renderer);

    RGB_From_Hexadecimal(fgc, &r, &g, &b);
    SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
}

static inline void game_world_reset(SDL_Renderer* renderer, SDL_Texture* texture, uint32_t fgc, uint32_t bgc) {
    SDL_SetRenderTarget(renderer, texture);
    game_world_reset(renderer, fgc, bgc);
}

static inline void game_world_refresh(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_SetRenderTarget(renderer, nullptr);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    SDL_SetRenderTarget(renderer, texture);
}

/*************************************************************************************************/
WarGrey::STEM::IUniverse::IUniverse(uint32_t fps, uint32_t fgc, uint32_t bgc) : _fgc(fgc), _bgc(bgc), _fps(fps), _mfgc(fgc) {
    // 初始化游戏系统
    game_initialize(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    game_create_world(1, 0, &this->window, &this->renderer);
    
    this->echo.x = 0;
    this->echo.h = 0;
}

WarGrey::STEM::IUniverse::~IUniverse() {
    if (this->timer > 0) {
        SDL_RemoveTimer(this->timer);
    }

    if (this->texture != nullptr) {
        SDL_DestroyTexture(this->texture);
    }

    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
}

void WarGrey::STEM::IUniverse::big_bang() {
    uint32_t quit_time = 0UL;           // 游戏退出时的在线时间
    timer_parcel_t parcel;              // 时间轴包裹
    SDL_Event e;                        // SDL 事件
    
    if (this->_fps > 0) {
        parcel.universe = this;
        parcel.interval = 1000 / this->_fps;

        this->timer = Call_For_Variable(timer,
                SDL_AddTimer(parcel.interval, trigger_timer_event, reinterpret_cast<void*>(&parcel)),
                0, "定时器创建失败: ", SDL_GetError);
    }

    this->feed_window_size(&this->window_width, &this->window_height);
    this->begin_update_sequence();
    this->on_big_bang(this->window_width, this->window_height - this->get_cmdwin_height());
    this->on_resize(this->window_width, this->window_height);
    this->on_game_start();
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
                    /** TODO
                     * Is SDL2 really pumping duplicate events?
                     * Why the first `count` is much larger then 1?
                     */
                    if (parcel->last_timestamp != parcel->uptime) {
                        this->on_elapse(parcel->count, parcel->interval, parcel->uptime);
                        parcel->last_timestamp = parcel->uptime;
                    }
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
            }
   
            this->end_update_sequence();
        }
    }
}

void WarGrey::STEM::IUniverse::on_mouse_event(SDL_MouseButtonEvent &mouse, bool pressed) {
    if (!pressed) {
        if (mouse.clicks == 1) {
            switch (mouse.button) {
                case SDL_BUTTON_LEFT: this->on_click(mouse.x, mouse.y); break;
                case SDL_BUTTON_RIGHT: this->on_right_click(mouse.x, mouse.y); break;
            }
        } else if (mouse.clicks == 2) {
            switch (mouse.button) {
                case SDL_BUTTON_LEFT: this->on_double_click(mouse.x, mouse.y); break;
            }
        }
    }
}

void WarGrey::STEM::IUniverse::on_mouse_event(SDL_MouseMotionEvent &mouse) {
    this->on_mouse_move(mouse.state, mouse.x, mouse.y, mouse.xrel, mouse.yrel);
}

void WarGrey::STEM::IUniverse::on_mouse_event(SDL_MouseWheelEvent &mouse) {
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

void WarGrey::STEM::IUniverse::on_keyboard_event(SDL_KeyboardEvent &keyboard, bool pressed) {
    SDL_Keysym key = keyboard.keysym;
    int ctrl_mod = KMOD_LCTRL | KMOD_RCTRL;

#if defined(__macosx__)
    ctrl_mod = ctrl_mod | KMOD_LGUI | KMOD_RGUI;
#endif

    if (this->in_editing) {
        if (!pressed) {
            if (key.mod & ctrl_mod) {
                switch (key.sym) {
                case SDLK_p: this->take_snapshot(); break;
                }
            } else {
                switch (key.sym) {
                case SDLK_RETURN: this->enter_input_text(); break;
                case SDLK_BACKSPACE: this->popback_input_text(); break;
                }
            }
        }
    } else {
        if (!pressed) {
            if (key.mod & ctrl_mod) {
                switch (key.sym) {
                case SDLK_s: this->save_file(is_shift_pressed()); break;
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

void WarGrey::STEM::IUniverse::on_resize(int width, int height) {
    this->window_width = width;
    this->window_height = height;

    if (this->echo.h > 0) {
        this->echo.y = height - this->echo.h;
        this->echo.w = width;
    }

    if (this->texture != nullptr) {
        SDL_DestroyTexture(this->texture);
    }

    this->texture = game_create_texture(this->window, this->renderer);
    
    this->begin_update_sequence();
    game_world_reset(this->renderer, this->texture, this->_fgc, this->_bgc);
    this->reflow(float(width), float(height - this->get_cmdwin_height()));
    this->notify_updated();
    this->end_update_sequence();
}

void WarGrey::STEM::IUniverse::on_user_input(const char* text) {
    if (this->in_editing) {
        this->usrin.append(text);
        this->current_usrin = nullptr;
        
        if (this->display_usr_input_and_caret(this->renderer, true)) {
            this->notify_updated();
        }
    }

    this->on_text(text, strlen(text), false);
}

void WarGrey::STEM::IUniverse::on_editing(const char* text, int pos, int span) {
    this->current_usrin = text;
    this->on_editing_text(text, pos, span);
}

void WarGrey::STEM::IUniverse::do_redraw(SDL_Renderer* renderer, int x, int y, int width, int height) {
    /**
     * Even if the subclass has its own background,
     *   resetting the renderer is also meaningful,
     *   as the `alpha` does't affect the renderer here,
     *      but does have effect for the subclass.
     */
    game_world_reset(renderer, this->_fgc, this->_bgc);
    this->draw(renderer, x, y, width, height - this->get_cmdwin_height());

    if (this->in_editing) {
        this->display_usr_input_and_caret(renderer, true);
    } else {
        this->display_usr_message(renderer);
    }
    
    if (this->echo.h > 0) {
        this->draw_cmdwin(renderer, this->echo.x, this->echo.y, this->echo.w, this->echo.h);
    }
}

void WarGrey::STEM::IUniverse::draw_cmdwin(SDL_Renderer* renderer, int x, int y, int width, int height) {
    game_draw_line(renderer, x, y, width, y, 0x888888U);
}

bool WarGrey::STEM::IUniverse::display_usr_message(SDL_Renderer* renderer) {
    bool updated = (this->echo.h > 0);

    if (updated) {
        game_fill_rect(renderer, &this->echo, this->_ibgc, 0xFF);

        if (!this->message.empty()) {
            game_draw_blended_text(this->echo_font, renderer, this->_mfgc,
                    this->echo.x, this->cmdline_message_yposition(), this->message, this->echo.w);
        }
    } else {
        if (!this->message.empty()) {
            if (this->needs_termio_if_no_echo) {
                printf("%s\n", this->message.c_str());
                this->needs_termio_if_no_echo = false;
            }
        }
    }

    return updated;
}

bool WarGrey::STEM::IUniverse::display_usr_input_and_caret(SDL_Renderer* renderer, bool yes) {
    bool updated = false;

    if (this->echo.h > 0) {
        game_fill_rect(renderer, &this->echo, this->_ibgc, 0xFF);

        if (yes) {
            if (this->prompt.empty()) {
                game_draw_blended_text(this->echo_font, renderer, this->_ifgc,
                        this->echo.x, this->cmdline_message_yposition(), this->usrin + "_", this->echo.w);
            } else {
                game_draw_blended_text(this->echo_font, renderer, this->_ifgc,
                        this->echo.x, this->cmdline_message_yposition(), this->prompt + this->usrin + "_", this->echo.w);
            }
        }

        updated = true;
    }

    return updated;
}

int WarGrey::STEM::IUniverse::cmdline_message_yposition() {
    int yoff = 0;

    if (this->echo_font->okay()) {
        yoff = (this->echo.h - this->echo_font->height()) / 2;
    }

    if (yoff < 0) {
        yoff = 0;
    }

    return this->echo.y + yoff;
}

/*************************************************************************************************/
void WarGrey::STEM::IUniverse::set_window_title(std::string& title) {
    SDL_SetWindowTitle(this->window, title.c_str());
}

void WarGrey::STEM::IUniverse::set_window_title(const char* fmt, ...) {
    VSNPRINT(title, fmt);
    this->set_window_title(title);
}

SDL_Renderer* WarGrey::STEM::IUniverse::master_renderer() {
    return this->renderer;
}

const char* WarGrey::STEM::IUniverse::get_renderer_name() {
    SDL_RendererInfo rinfo;

    SDL_GetRendererInfo(this->renderer, &rinfo);
    
    return rinfo.name; 
}

void WarGrey::STEM::IUniverse::set_window_size(int width, int height, bool centerize) {
    if ((width <= 0) || (height <= 0)) {
        int oldw, oldh;

        SDL_GetWindowSize(this->window, &oldw, &oldh);

        if (width <= 0) {
            width = oldw;
        }

        if (height <= 0) {
            height = oldh;
        }
    }

    SDL_SetWindowSize(this->window, width, height);

    if (centerize) {
        SDL_SetWindowPosition(this->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    if (this->texture != nullptr) {
        // the universe has been completely initialized
        this->on_resize(width, height);
    } else {
        // the big_bang() will do resizing later
    }
}

void WarGrey::STEM::IUniverse::feed_window_size(int* width, int* height, bool logical) {
    if (logical) {
        SDL_GetRendererOutputSize(this->renderer, width, height);
    } else {
        SDL_GetWindowSize(this->window, width, height);
    }
}

void WarGrey::STEM::IUniverse::set_window_fullscreen(bool yes) {
    if (yes) {
        SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(this->window, 0);
    }
}

void WarGrey::STEM::IUniverse::refresh() {
    this->do_redraw(this->renderer, 0, 0, this->window_width, this->window_height);
    game_world_refresh(this->renderer, this->texture);
}

void WarGrey::STEM::IUniverse::feed_extent(float* width, float* height) {
    int fxw, fxh;

    this->feed_window_size(&fxw, &fxh);
    SET_VALUES(width, float(fxw), height, float(fxh));
}

void WarGrey::STEM::IUniverse::feed_client_extent(float* width, float* height) {
    int fxw, fxh;

    this->feed_window_size(&fxw, &fxh);
    SET_BOX(width, float(fxw));
    SET_BOX(height, float(fxh - this->get_cmdwin_height()));
}

/*************************************************************************************************/
void WarGrey::STEM::IUniverse::set_cmdwin_height(int cmdwinheight, int fgc, int bgc, shared_font_t font) {
    int width, height;

    this->feed_window_size(&width, &height);

    this->echo.y = height - cmdwinheight;
    this->echo.h = cmdwinheight;

    this->_ifgc = (fgc < 0) ? this->_fgc : fgc;
    this->_ibgc = (bgc < 0) ? this->_bgc : bgc;
    this->echo_font = font;

    SDL_SetTextInputRect(&this->echo);
}

void WarGrey::STEM::IUniverse::log_message(int fgc, const std::string& msg) {
    this->needs_termio_if_no_echo = true;
    this->message = msg;
    
    if (fgc >= 0) {
        this->_mfgc = fgc;
    }

    if (this->display_usr_message(this->renderer)){
        this->notify_updated();
    }
}

void WarGrey::STEM::IUniverse::start_input_text(const char* fmt, ...) {
    VSNPRINT(p, fmt);
    this->start_input_text(p);
}

void WarGrey::STEM::IUniverse::start_input_text(const std::string& p) {
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

void WarGrey::STEM::IUniverse::stop_input_text() {
    this->in_editing = false;
    SDL_StopTextInput();
    this->on_text(this->usrin.c_str(), this->usrin.size(), true);
    this->usrin.erase();
    this->prompt.erase();

    if (this->display_usr_input_and_caret(this->renderer, false)) {
        this->notify_updated();
    }
}

void WarGrey::STEM::IUniverse::enter_input_text() {
    if (this->current_usrin != nullptr) {
        this->on_user_input(this->current_usrin);
        this->current_usrin = nullptr;
    } else {
        this->stop_input_text();
    }
}

void WarGrey::STEM::IUniverse::popback_input_text() {
    if (string_popback_utf8_char(this->usrin)) {
        if (this->display_usr_input_and_caret(this->renderer, true)) {
            this->notify_updated();
        }
    }
}

/*************************************************************************************************/
SDL_Surface* WarGrey::STEM::IUniverse::snapshot() {
    uint32_t format = SDL_PIXELFORMAT_RGBA8888;
    SDL_Surface* photograph = game_formatted_surface(this->window_width, this->window_height, format);

    if (photograph != nullptr) {
        if (SDL_RenderReadPixels(this->renderer, NULL, format, photograph->pixels, photograph->pitch) < 0) {
            this->log_message(0xFF0000, "failed to take snapshot: %s", SDL_GetError());
        }
    }

    return photograph;
}

void WarGrey::STEM::IUniverse::take_snapshot() {
    const char* basename = SDL_GetWindowTitle(this->window);
    path snapshot_png = (this->snapshot_rootdir.empty() ? current_path() : path(this->snapshot_rootdir))
        / path(game_create_string("%s-%s.png", basename, make_now_timestamp_utc(true).c_str()));

    if (this->save_snapshot(snapshot_png.string().c_str())) { // stupid windows as it requires `string()`
        this->log_message(this->_fgc, "A snapshot has been saved as '%s'.", snapshot_png.string().c_str());
    } else {
        this->log_message(0xFF0000, "failed to save snapshot: %s", SDL_GetError());
    }
}

void WarGrey::STEM::IUniverse::set_snapshot_folder(const char* dir) {
    this->set_snapshot_folder(std::string(dir));
}

void WarGrey::STEM::IUniverse::set_snapshot_folder(const std::string& dir) {
    this->snapshot_rootdir = path(dir).make_preferred().string();
}

void WarGrey::STEM::IUniverse::save_file(bool is_save_as) {
    const char* ext = this->usrdata_extension();

    if (ext != nullptr) {
        path data_path = this->usrdata_rootdir.empty() ? current_path() : path(this->usrdata_rootdir);
        const char* basename = SDL_GetWindowTitle(this->window);
    
        if (!is_save_as) {
            data_path /= path(game_create_string("%s%s", basename, ext));
        } else {
            data_path /= path(game_create_string("%s-%s%s", basename, make_now_timestamp_utc(true).c_str(), ext));
        }

        try {
            std::ofstream dev_datout;

            dev_datout.exceptions(std::ofstream::badbit | std::ofstream::failbit);
            dev_datout.open(data_path.string(), std::ofstream::out);
            this->on_save(data_path.string(), dev_datout);
            dev_datout.close();

            this->log_message(this->_fgc, "The user data have been saved as '%s'.", data_path.string().c_str());
        } catch (const std::ofstream::failure &e) {
            this->log_message(0xFF0000, "failed to save user data: %s", e.what());
        }
    }
}

void WarGrey::STEM::IUniverse::set_usrdata_folder(const char* dir) {
    this->set_usrdata_folder(std::string(dir));
}

void WarGrey::STEM::IUniverse::set_usrdata_folder(const std::string& dir) {
    this->usrdata_rootdir = path(dir).make_preferred().string();
}

/*************************************************************************************************/
WarGrey::STEM::Universe::Universe() : Universe("The Big Bang!") {}
WarGrey::STEM::Universe::Universe(const char *title, uint32_t fps, uint32_t fgc, uint32_t bgc) : IUniverse(fps, fgc, bgc) {
    this->set_window_title("%s", title);
}

void WarGrey::STEM::Universe::on_elapse(uint64_t count, uint32_t interval, uint64_t uptime) {
    this->update(count, interval, uptime);
    this->notify_updated();
}
