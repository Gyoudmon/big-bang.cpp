#include "universe.hpp"
#include "misc.hpp"

#include "graphics/image.hpp"
#include "physics/color/rgba.hpp"
#include "physics/color/names.hpp"

#include "datum/string.hpp"
#include "datum/box.hpp"
#include "datum/time.hpp"

#include <SDL2/SDL_mixer.h>

#include <filesystem>

using namespace GYDM;
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
    
    /* Initializing SDL2_IMG for loading images */ {
#if defined(__macosx__)
        int img_request_flags = IMG_INIT_JPG | IMG_INIT_PNG;
#else
        int img_request_flags = IMG_INIT_PNG;
#endif
        int img_okay_flags = IMG_Init(img_request_flags);

        if ((img_okay_flags & img_request_flags) != img_request_flags) {
            fprintf(stderr, "IMG 初始化失败: %s\n", IMG_GetError());
            exit(1);
        }
    }

    /* Initialize SDL2_Mixer for playing music */ {
#if defined(__macosx__)
        int mix_request_flags = MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MID | MIX_INIT_MOD | MIX_INIT_FLAC;
#else
        int mix_request_flags = MIX_INIT_MP3 | MIX_INIT_OGG;
#endif
        int mix_okay_flags = Mix_Init(mix_request_flags);

        if ((mix_okay_flags & mix_request_flags) != mix_request_flags) {
            fprintf(stderr, "Mix 初始化失败: %s\n", Mix_GetError());
        }
    }

    game_fonts_initialize();

    atexit(Mix_Quit);
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

/*************************************************************************************************/
GYDM::IUniverse::IUniverse(uint32_t fps, const RGBA& fgc, const RGBA& bgc) : _fgc(fgc), _bgc(bgc), _fps(fps), _mfgc(fgc) {
    SDL_Renderer* renderer;

    // 初始化游戏系统
    game_initialize(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    game_create_world(1, 0, &this->window, &renderer);
    
    this->device = new DrawingContext(renderer);
    this->echo.x = 0;
    this->echo.h = 0;
}

GYDM::IUniverse::~IUniverse() {
    if (this->timer > 0) {
        SDL_RemoveTimer(this->timer);
    }

    if (this->texture != nullptr) {
        SDL_DestroyTexture(this->texture);
    }

    delete this->device;
    SDL_DestroyWindow(this->window);
}

void GYDM::IUniverse::big_bang() {
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
        } else {
            this->log_message(Log::Error, make_nstring("failed to pop the event: %s", SDL_GetError()));
        }
    }
}

void GYDM::IUniverse::on_mouse_event(SDL_MouseButtonEvent &mouse, bool pressed) {
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

void GYDM::IUniverse::on_mouse_event(SDL_MouseMotionEvent &mouse) {
    this->on_mouse_move(mouse.state, mouse.x, mouse.y, mouse.xrel, mouse.yrel);
}

void GYDM::IUniverse::on_mouse_event(SDL_MouseWheelEvent &mouse) {
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

void GYDM::IUniverse::on_keyboard_event(SDL_KeyboardEvent &keyboard, bool pressed) {
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

void GYDM::IUniverse::on_resize(int width, int height) {
    this->window_width = width;
    this->window_height = height;

    if (this->echo.h > 0) {
        this->echo.y = height - this->echo.h;
        this->echo.w = width;
    }

    if (this->texture != nullptr) {
        SDL_DestroyTexture(this->texture);
    }

    this->texture = game_create_texture(this->window, this->device->self());
    
    this->begin_update_sequence();
    this->device->reset(this->texture, this->_fgc, this->_bgc);
    this->reflow(float(width), float(height - this->get_cmdwin_height()));
    this->notify_updated();
    this->end_update_sequence();
}

void GYDM::IUniverse::on_user_input(const char* text) {
    if (this->in_editing) {
        this->usrin.append(text);
        this->current_usrin = nullptr;
        
        if (this->display_usr_input_and_caret(this->device, true)) {
            this->notify_updated();
        }
    }

    this->on_text(text, strlen(text), false);
}

void GYDM::IUniverse::on_editing(const char* text, int pos, int span) {
    this->current_usrin = text;
    this->on_editing_text(text, pos, span);
}

void GYDM::IUniverse::do_redraw(dc_t* device, int x, int y, int width, int height) {
    /**
     * Even if the subclass has its own background,
     *   resetting the renderer is also meaningful,
     *   as the `alpha` does't affect the renderer here,
     *      but does have effect for the subclass.
     */
    device->reset(this->_fgc, this->_bgc);
    this->draw(device, x, y, width, height - this->get_cmdwin_height());

    if (this->in_editing) {
        this->display_usr_input_and_caret(device, true);
    } else {
        this->display_usr_message(device);
    }
    
    if (this->echo.h > 0) {
        this->draw_cmdwin(device, this->echo.x, this->echo.y, this->echo.w, this->echo.h);
    }
}

void GYDM::IUniverse::draw_cmdwin(dc_t* device, int x, int y, int width, int height) {
    device->draw_line(x, y, width, y, 0x888888U);
}

bool GYDM::IUniverse::display_usr_message(dc_t* device) {
    bool updated = (this->echo.h > 0);

    if (updated) {
        device->fill_rect(&this->echo, this->_ibgc);

        if (!this->message.empty()) {
            device->draw_blended_text(this->message, this->echo_font,
                    this->echo.x, this->cmdline_message_yposition(),
                    this->_mfgc, this->echo.w);
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

bool GYDM::IUniverse::display_usr_input_and_caret(dc_t* device, bool yes) {
    bool updated = false;

    if (this->echo.h > 0) {
        device->fill_rect(&this->echo, this->_ibgc);

        if (yes) {
            if (this->prompt.empty()) {
                device->draw_blended_text(this->usrin + "_", this->echo_font,
                        this->echo.x, this->cmdline_message_yposition(),
                        this->_ifgc, this->echo.w);
            } else {
                device->draw_blended_text(this->prompt + this->usrin + "_", this->echo_font,
                        this->echo.x, this->cmdline_message_yposition(),
                        this->_ifgc, this->echo.w);
            }
        }

        updated = true;
    }

    return updated;
}

int GYDM::IUniverse::cmdline_message_yposition() {
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
void GYDM::IUniverse::set_window_title(std::string& title) {
    SDL_SetWindowTitle(this->window, title.c_str());
}

void GYDM::IUniverse::set_window_title(const char* fmt, ...) {
    VSNPRINT(title, fmt);
    this->set_window_title(title);
}

dc_t* GYDM::IUniverse::drawing_context() {
    return this->device;
}

void GYDM::IUniverse::set_window_size(int width, int height, bool centerize) {
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

void GYDM::IUniverse::feed_window_size(int* width, int* height, bool logical) {
    if (logical) {
        this->device->feed_output_size(width, height);
    } else {
        SDL_GetWindowSize(this->window, width, height);
    }
}

void GYDM::IUniverse::set_window_fullscreen(bool yes) {
    if (yes) {
        SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(this->window, 0);
    }
}

void GYDM::IUniverse::refresh() {
    this->do_redraw(this->device, 0, 0, this->window_width, this->window_height);
    this->device->refresh(this->texture);
}

void GYDM::IUniverse::feed_extent(float* width, float* height) {
    int fxw, fxh;

    this->feed_window_size(&fxw, &fxh);
    SET_VALUES(width, float(fxw), height, float(fxh));
}

void GYDM::IUniverse::feed_client_extent(float* width, float* height) {
    int fxw, fxh;

    this->feed_window_size(&fxw, &fxh);
    SET_BOX(width, float(fxw));
    SET_BOX(height, float(fxh - this->get_cmdwin_height()));
}

/*************************************************************************************************/
void GYDM::IUniverse::set_cmdwin_height(int cmdwinheight, int fgc, int bgc, shared_font_t font) {
    int width, height;

    this->feed_window_size(&width, &height);

    this->echo.y = height - cmdwinheight;
    this->echo.h = cmdwinheight;

    this->_ifgc = (fgc < 0) ? this->_fgc : fgc;
    this->_ibgc = (bgc < 0) ? this->_bgc : bgc;
    this->echo_font = font;

    SDL_SetTextInputRect(&this->echo);
}

void GYDM::IUniverse::log_message(Log level, const std::string& msg) {
    this->needs_termio_if_no_echo = true;
    this->message = msg;
    
    switch (level) {
    case Log::Fatal: this->_mfgc = CRIMSON; break;
    case Log::Error: this->_mfgc = RED; break;
    case Log::Warning: this->_mfgc = YELLOW; break;
    case Log::Info: this->_mfgc = GREEN; break;
    case Log::Debug: this->_mfgc = SILVER; break;
    default: this->_mfgc = transparent; break;
    }

    if (this->display_usr_message(this->device)){
        this->notify_updated();
    }
}

void GYDM::IUniverse::start_input_text(const char* fmt, ...) {
    VSNPRINT(p, fmt);
    this->start_input_text(p);
}

void GYDM::IUniverse::start_input_text(const std::string& p) {
    if (p.size() > 0) {
        this->prompt = p;
    }

    SDL_StartTextInput();
    this->in_editing = true;
    this->message.erase();
    
    if (this->display_usr_input_and_caret(this->device, true)) {
        this->notify_updated();
    }
}

void GYDM::IUniverse::stop_input_text() {
    this->in_editing = false;
    SDL_StopTextInput();
    this->on_text(this->usrin.c_str(), this->usrin.size(), true);
    this->usrin.erase();
    this->prompt.erase();

    if (this->display_usr_input_and_caret(this->device, false)) {
        this->notify_updated();
    }
}

void GYDM::IUniverse::enter_input_text() {
    if (this->current_usrin != nullptr) {
        this->on_user_input(this->current_usrin);
        this->current_usrin = nullptr;
    } else {
        this->stop_input_text();
    }
}

void GYDM::IUniverse::popback_input_text() {
    if (string_popback_utf8_char(this->usrin)) {
        if (this->display_usr_input_and_caret(this->device, true)) {
            this->notify_updated();
        }
    }
}

/*************************************************************************************************/
SDL_Surface* GYDM::IUniverse::snapshot() {
    SDL_Surface* photograph = this->device->snapshot(this->window_width, this->window_height);

    if (photograph == nullptr) {
        this->log_message(Log::Error, make_nstring("failed to take snapshot: %s", SDL_GetError()));
    }

    return photograph;
}

void GYDM::IUniverse::take_snapshot() {
    const char* basename = SDL_GetWindowTitle(this->window);
    path snapshot_png = (this->snapshot_rootdir.empty() ? current_path() : path(this->snapshot_rootdir))
        / path(make_nstring("%s-%s.png", basename, make_now_timestamp_utc(true).c_str()));

    if (this->save_snapshot(snapshot_png.string().c_str())) { // stupid windows as it requires `string()`
        this->log_message(Log::Info, make_nstring("A snapshot has been saved as '%s'.", snapshot_png.string().c_str()));
    } else {
        this->log_message(Log::Error, make_nstring("failed to save snapshot: %s", SDL_GetError()));
    }
}

void GYDM::IUniverse::set_snapshot_folder(const char* dir) {
    this->set_snapshot_folder(std::string(dir));
}

void GYDM::IUniverse::set_snapshot_folder(const std::string& dir) {
    this->snapshot_rootdir = path(dir).make_preferred().string();
}

void GYDM::IUniverse::save_file(bool is_save_as) {
    const char* ext = this->usrdata_extension();

    if (ext != nullptr) {
        path data_path = this->usrdata_rootdir.empty() ? current_path() : path(this->usrdata_rootdir);
        const char* basename = SDL_GetWindowTitle(this->window);
    
        if (!is_save_as) {
            data_path /= path(make_nstring("%s%s", basename, ext));
        } else {
            data_path /= path(make_nstring("%s-%s%s", basename, make_now_timestamp_utc(true).c_str(), ext));
        }

        try {
            std::ofstream dev_datout;

            dev_datout.exceptions(std::ofstream::badbit | std::ofstream::failbit);
            dev_datout.open(data_path.string(), std::ofstream::out);
            this->on_save(data_path.string(), dev_datout);
            dev_datout.close();

            this->log_message(Log::Info, make_nstring("The user data have been saved as '%s'.", data_path.string().c_str()));
        } catch (const std::ofstream::failure &e) {
            this->log_message(Log::Error, make_nstring("failed to save user data: %s", e.what()));
        }
    }
}

void GYDM::IUniverse::set_usrdata_folder(const char* dir) {
    this->set_usrdata_folder(std::string(dir));
}

void GYDM::IUniverse::set_usrdata_folder(const std::string& dir) {
    this->usrdata_rootdir = path(dir).make_preferred().string();
}

/*************************************************************************************************/
GYDM::Universe::Universe() : Universe("The Big Bang!") {}
GYDM::Universe::Universe(const char *title, uint32_t fps, const RGBA& fgc, const RGBA& bgc) : IUniverse(fps, fgc, bgc) {
    this->set_window_title("%s", title);
}

void GYDM::Universe::on_elapse(uint64_t count, uint32_t interval, uint64_t uptime) {
    this->update(count, interval, uptime);
    this->notify_updated();
}
