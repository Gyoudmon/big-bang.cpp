#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <cstdint>

#include "font.hpp"
#include "image.hpp"

#include "display.hpp"

namespace WarGrey::STEM {
    class IUniverse : public WarGrey::STEM::IDisplay {
        public:
            /* 构造函数，用以设置帧频, 窗口标题, 前景背景色, 和混色模式 */
            IUniverse(int fps, uint32_t fgc, uint32_t bgc);
            
            /* 析构函数，销毁旧对象时自动调用，默认销毁游戏宇宙 */
            virtual ~IUniverse();

            /* 宇宙大爆炸，开始游戏主循环 */
            void big_bang();

        public:
            /* 创建游戏世界，充当程序真正的 main 函数 */
            virtual void construct(int argc, char* argv[]) = 0;
            
            /* 排列可视化元素，在合适的时候自动调用 */
            virtual void reflow(float window, float height) = 0;

            /* 更新游戏世界，定时器到期时自动调用 */
            virtual void update(uint32_t interval, uint32_t count, uint32_t uptime) = 0;
            
            /* 绘制游戏世界，在合适的时候自动调用 */
            virtual void draw(SDL_Renderer* renderer, int x, int y, int width, int height) = 0;

            /* 告诉游戏主循环，是否游戏已经结束可以退出了，默认永久运行 */
            virtual bool can_exit() { return false; }

        public: // 常规操作
            void set_snapshot_folder(const char* path);
            void set_snapshot_folder(const std::string& path);
            SDL_Surface* snapshot() override;
            
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

        public: // 窗体相关方法
            void refresh() override;
            void fill_extent(float* width, float* height) override;

        public: // 用户 IME 输入输出
            void set_cmdwin_height(int height, int fgc = -1, int bgc = -1, TTF_Font* font = game_font::unicode);
            int get_cmdwin_height() { return this->echo.h; }
            void start_input_text(const char* fmt, ...);
            void start_input_text(const std::string& prompt) override;
            void stop_input_text();
            
            using WarGrey::STEM::IDisplay::log_message;
            void log_message(int fgc, const std::string& message) override;

        protected: // 常规事件处理和分派函数
            virtual void on_click(int x, int y) {}                                               // 处理单击事件
            virtual void on_right_click(int x, int y) {}                                         // 处理右击事件
            virtual void on_double_click(int x, int y) {}                                        // 处理双击事件
            virtual void on_mouse_move(uint32_t state, int x, int y, int dx, int dy) {}          // 处理移动事件
            virtual void on_scroll(int horizon, int vertical, float hprecise, float vprecise) {} // 处理滚轮事件

            virtual void on_char(char key, uint16_t modifiers, uint8_t repeats, bool pressed) {} // 处理键盘事件
            virtual void on_text(const char* text, size_t size, bool entire) {}                  // 处理文本输入事件
            virtual void on_editing_text(const char* text, int pos, int span) {}                         // 处理文本输入事件
            
            virtual void on_save() {}                                                            // 处理保存事件

        protected:
            virtual void draw_cmdwin(SDL_Renderer* renderer, int x, int y, int width, int height);

        protected:
            /* 大爆炸之前最后的初始化宇宙机会，默认什么都不做 */
            virtual void on_big_bang(int width, int height) {}

            /* 响应定时器事件，刷新游戏世界 */
            virtual void on_elapse(uint32_t interval, uint32_t count, uint32_t uptime) = 0;

            /* 响应鼠标事件，并按需触发单击、右击、双击、移动、滚轮事件 */
            virtual void on_mouse_event(SDL_MouseButtonEvent& mouse, bool pressed); 
            virtual void on_mouse_event(SDL_MouseMotionEvent& mouse); 
            virtual void on_mouse_event(SDL_MouseWheelEvent& mouse);

            /* 响应键盘事件，并按需触发按下、松开事件 */
            virtual void on_keyboard_event(SDL_KeyboardEvent& key, bool pressed);

            /* 响应窗体事件，并按需触发尺寸改变事件 */
            virtual void on_resize(int width, int height);

            /* 响应输入法事件，按需显示用户输入的内容 */
            virtual void on_user_input(const char* text);
            virtual void on_editing(const char* text, int pos, int span);

            /* 处理预设事件  */
            virtual void take_snapshot();
        
        private:
            void do_redraw(SDL_Renderer* renderer, int x, int y, int width, int height);
            bool display_usr_input_and_caret(SDL_Renderer* renderer, bool yes);
            bool display_usr_message(SDL_Renderer* renderer);
            void enter_input_text();
            void popback_input_text();

        private:
            uint32_t _fgc;                          // 窗体前景色
            uint32_t _bgc;                          // 窗体背景色
            int window_width;                       // 窗体宽度
            int window_height;                      // 窗体高度
            SDL_Window* window = nullptr;           // 窗体对象
            SDL_Renderer* renderer = nullptr;       // 渲染器对象
            SDL_Texture* texture = nullptr;         // 纹理对象

        private:
            SDL_TimerID timer;                      // SDL 定时器
            int _fps;                               // 帧频

        private:
            const char* current_usrin = nullptr;    // IME 原始输入
            std::string prompt;                     // 输入提示
            std::string usrin;                      // 用户输入
            bool in_editing = false;                // 是否在输入期间
            SDL_Rect echo;                          // 输入回显区域
            uint32_t _ifgc;                         // 回显区前景色
            uint32_t _ibgc;                         // 回显区背景色
            TTF_Font* echo_font = nullptr;          // 回显字体
            std::string message;                    // 回显区消息
            uint32_t _mfgc;                         // 消息颜色
            bool needs_termio_if_no_echo;           // 消息是否需要输出

        private:
            std::string snapshot_rootdir;           // 屏幕截图位置
    };

    class Universe : public WarGrey::STEM::IUniverse {
        public:
            /* 构造函数，创建新对象时自动调用，默认创建一个黑底白字的窗口 */
            Universe();

            /* 更有用一些的构造函数，创建新对象时根据参数自动选择 */
            Universe(const char* title, int fps = 60, uint32_t fgc = 0x000000U, uint32_t bgc = 0xFFFFFFU);
        
        public:
            /* 创建游戏世界，充当程序真正的 main 函数，默认什么都不做 */
            void construct(int argc, char* argv[]) override {}
            
            /* 排列可视化元素，在合适的时候自动调用，默认什么都不做 */
            void reflow(float window, float height) override {}

            /* 更新游戏世界，定时器到期时自动调用，默认什么都不做 */
            void update(uint32_t interval, uint32_t count, uint32_t uptime) override {}
            
            /* 绘制游戏世界，在合适的时候自动调用，默认什么都不做 */
            void draw(SDL_Renderer* renderer, int x, int y, int width, int height) override {}

        protected:
            void on_elapse(uint32_t interval, uint32_t count, uint32_t uptime) override;
    };
}

