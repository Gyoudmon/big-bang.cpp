#ifndef _SYNTAX_GAME_H
#define _SYNTAX_GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <cstdint>
#include <string>

#include "named_colors.hpp"
#include "geometry.hpp"

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

namespace WarGrey::STEM {
    /**********************************************************************************************/
    extern TTF_Font* GAME_DEFAULT_FONT;
    extern TTF_Font* game_sans_serif_font;
    extern TTF_Font* game_serif_font;
    extern TTF_Font* game_monospace_font;
    extern TTF_Font* game_math_font;
    extern TTF_Font* game_unicode_font;

    TTF_Font* game_create_font(const char* face, int fontsize);
    void game_font_destroy(TTF_Font* font);
    const std::string* game_font_list(int* n, int fontsize = 16);
    
    /**********************************************************************************************/
    class Universe {
        public:
            /* 构造函数，创建新对象时自动调用，默认创建一个黑底白字的窗口 */
            Universe();

            /**
             * 更有用一些的构造函数，创建新对象时根据参数自动选择，
             * 设置帧频, 窗口标题, 前景背景色, 和混色模式
             */
            Universe(const char* title, int fps = 60, uint32_t fgc = 0xFFFFFFU, uint32_t bgc = 0x000000U);
            
            /* 析构函数，销毁旧对象时自动调用，默认销毁游戏宇宙 */
            virtual ~Universe();

            /* 宇宙大爆炸，开始游戏主循环 */
            void big_bang();

        public:
            /* 创建游戏世界，充当程序真正的 main 函数，默认什么都不做 */
            virtual void construct(int argc, char* argv[]) {}
            
            /* 排列可视化元素，在合适的时候自动调用，默认什么都不做 */
            virtual void reflow(int width, int height) {}
            
            /* 更新游戏世界，定时器到期时自动调用，默认什么都不做 */
            virtual void update(uint32_t interval, uint32_t count, uint32_t uptime) {}
            
            /* 绘制游戏世界，在合适的时候自动调用，默认什么都不做 */
            virtual void draw(SDL_Renderer* renderer, int x, int y, int width, int height) {}

            /* 告诉游戏主循环，是否游戏已经结束可以退出了，默认永久运行 */
            virtual bool can_exit() { return false; }

        public: // 常规操作
            void set_snapshot_folder(const char* path);
            void set_snapshot_folder(const std::string& path);
            bool snapshot(const std::string& path);
            bool snapshot(const char* path);
            
        public: // 窗体 setter 和 getter
            void set_blend_mode(SDL_BlendMode bmode);
            void set_window_title(std::string& title);
            void set_window_title(const char* fmt, ...);
            void set_window_size(int width, int height, bool centerize = true);
            void fill_window_size(int* width, int* height);
            void set_window_fullscreen(bool yes);
            int get_frame_per_second() { return this->_fps; }
            uint32_t get_background_color() { return this->_bgc; }
            uint32_t get_foreground_color() { return this->_fgc; }

        public: // 窗体重汇相关
            void refresh();
            void begin_update_sequence() { this->update_sequence_depth += 1; }
            bool in_update_sequence() { return (this->update_sequence_depth > 0); }
            bool needs_update() { return this->update_is_needed; }
            void end_update_sequence();
            void notify_updated();

        public: // 用户 IME 输入输出
            void set_input_echo_area(int x, int y, int width, int height, int fgc = -1, int bgc = -1, TTF_Font* font = game_unicode_font);
            void start_input_text(const char* fmt, ...);
            void start_input_text(const std::string& prompt);
            void stop_input_text();

            void send_message(uint32_t fgc, const char* fmt, ...);
            void send_message(uint32_t fgc, const std::string& msg);
            void send_message(const char* fmt, ...);

        protected: // 常规事件处理和分派函数
            virtual void on_click(int x, int y) {}                                               // 处理单击事件
            virtual void on_right_click(int x, int y) {}                                         // 处理右击事件
            virtual void on_double_click(int x, int y) {}                                        // 处理双击事件
            virtual void on_mouse_move(uint32_t state, int x, int y, int dx, int dy) {}          // 处理移动事件
            virtual void on_scroll(int horizon, int vertical, float hprecise, float vprecise) {} // 处理滚轮事件

            virtual void on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) {} // 处理键盘事件
            virtual void on_text(const char* text, bool entire) {}                               // 处理文本输入事件
            virtual void on_text(const char* text, int pos, int span) {}                         // 处理文本输入事件
            
            virtual void save() {}                                                               // 处理保存事件                                  

        private:
            /* 响应定时器事件，刷新游戏世界 */
            void on_elapse(uint32_t interval, uint32_t count, uint32_t uptime);

            /* 响应鼠标事件，并按需调用单击、右击、双击、移动、滚轮事件 */
            void on_mouse_event(SDL_MouseButtonEvent &mouse, bool pressed); 
            void on_mouse_event(SDL_MouseMotionEvent &mouse); 
            void on_mouse_event(SDL_MouseWheelEvent &mouse);

            /* 响应键盘事件，并按需调按下、松开事件 */
            void on_keyboard_event(SDL_KeyboardEvent &key, bool pressed);

            /* 响应窗体事件，并按需调用尺寸改变事件 */
            void on_resize(int width, int height);

            /* 响应输入法事件，按需显示用户输入的内容 */
            void on_user_input(const char* text);
            void on_editing(const char* text, int pos, int span);

            /* 处理预设事件  */
            void take_snapshot();
        
        private:
            void do_redraw(SDL_Renderer* renderer, int x, int y, int width, int height);
            bool display_usr_input_and_caret(SDL_Renderer* renderer, bool yes);
            bool display_usr_message();
            void enter_input_text();
            void popback_input_text();

        private:
            uint32_t _fgc = 0xFFFFFFU;      // 窗体前景色
            uint32_t _bgc = 0x000000U;      // 窗体背景色
            int window_width = 0;           // 窗体宽度
            int window_height = 0;          // 窗体高度
            SDL_Window* window = NULL;      // 窗体对象
            SDL_Renderer* renderer = NULL;  // 渲染器对象
            SDL_Texture* texture = NULL;    // 纹理对象

        private:
            SDL_TimerID timer = 0;          // SDL 定时器
            int _fps = 60;                  // 帧频

        private:
            const char* current_usrin;      // IME 原始输入
            std::string prompt;             // 输入提示
            std::string usrin;              // 用户输入
            bool in_editing;                // 是否在输入期间
            SDL_Rect echo;                  // 输入回显区域
            uint32_t _ifgc;                 // 回显区前景色
            uint32_t _ibgc;                 // 回显区背景色
            TTF_Font* echo_font;            // 回显字体

            std::string message;            // 回显区消息
            uint32_t _mfgc;                 // 消息颜色
            bool needs_termio_if_no_echo;   // 消息是否需要输出

        private:
            int update_sequence_depth;      // 延迟绘制深度
            bool update_is_needed;          // 有绘制事件被延迟

        private:
            std::string snapshot_rootdir;   // 屏幕截图位置
    };

    class Pasteboard : public WarGrey::STEM::Universe {
        public:
            Pasteboard(const char* title, uint32_t fgc = 0x000000U, uint32_t bgc = 0xFFFFFFU)
                : Universe(title, 0, fgc, bgc) {}
    };
}

#endif

