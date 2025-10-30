#include "consolet.hpp"

#include "../graphlet/textlet.hpp"
#include "../../plane.hpp"

using namespace Plteen;

/*************************************************************************************************/
class Plteen::Consolet::GhostConsolePlane : public Plane {
    friend Plteen::Consolet;
public:
    GhostConsolePlane(shared_font_t ft, int r, int c, const char* name) : Plane(name), term_font(ft), term_row(r), term_col(c) {
        if ((r > 0) || (c > 0)) {
            this->chars = new Labellet**[this->term_row + 1];
        
            for (int r = 0; r <= this->term_row; r ++) {
                this->chars[r] = new Labellet*[this->term_col + 1];
            }
        }
    }

    virtual ~GhostConsolePlane() noexcept {
        if (this->chars != nullptr) {
            for (int r = 0; r <= this->term_row; r ++) {
                delete [] this->chars[r];
            }

            delete [] this->chars;
        }
    }

public:
    void load(float width, float height) override {
        auto cell_seq_font = GameFont::Default(FontSize::x_small);

        for (int r = 0; r < this->term_row; r ++) {
            for (int c = 0; c < this->term_col; c ++) {
                this->chars[r][c] = this->insert(new Labellet(this->term_font, DIMGRAY));
            }
        }

        this->cursor = this->insert(new Labellet(this->term_font, ROYALBLUE));

        for (int r = 0; r < this->term_row; r ++) {
            this->chars[r][this->term_col] = this->insert(new Labellet(cell_seq_font, SILVER, "%d", r + 1));
        }

        for (int c = 0; c < this->term_col; c ++) {
            this->chars[this->term_row][c] = this->insert(new Labellet(cell_seq_font, SILVER, "%d", c + 1));
        }
    }

    void reflow(float width, float height) override {

    }
    
public:
    void sync_cursor() {
        this->term_cidx = 0;
        this->term_ridx ++;
    
    while (this->term_ridx >= this->term_row) {
        this->try_scroll_screen();
        this->term_ridx --;
    }

    if (sync) {
        this->sync_cursor();
    }
    }

    void linefeed(bool sync = true) {
        this->glide_to_grid(CURSOR_DURATION, this->cursor, this->term_ridx, this->term_cidx, MatterPort::CC);
    }

    void display(const std::string& message, bool sync_cursor = true) {
        if (this->term_ridx < this->term_row) {
            size_t i = 0;

            while (i < message.size()) {
                size_t len = string_character_size(message, i);
                std::string character = message.substr(i, len);
            
                i += len;

                if (character != "\n") {
                    if (len == 1) {
                        this->chars[this->term_ridx][this->term_cidx]->show(true);
                        this->chars[this->term_ridx][this->term_cidx]->set_text(character, MatterPort::CC);
                        this->move_to_grid(this->chars[this->term_ridx][this->term_cidx], this->term_ridx, this->term_cidx, MatterPort::CC);
                        this->term_cidx ++;
                    } else {
                        this->clear_term_cell(this->term_ridx, this->term_cidx, 2);

                        if (this->term_cidx + 2 > this->term_col) {
                            this->linefeed(false);
                        }

                        this->chars[this->term_ridx][this->term_cidx]->show(true);
                        this->chars[this->term_ridx][this->term_cidx]->set_text(character, MatterPort::CC);
                        this->glide_to_grid(CHAR_DURATION, this->chars[this->term_ridx][this->term_cidx],
                                                this->term_ridx, this->term_cidx, MatterPort::CC,
                                                { GRID_CELL_WIDTH * 0.5F, 0.0F });
                        this->term_cidx += 2;
                    }
                }
        
                if ((character == "\n") || (this->term_cidx >= this->term_col)) {
                    this->linefeed(false);
                }
            }

            if (sync_cursor) {
                this->sync_cursor();
            }
        }
    }

    void clear_screen() {
        this->term_ridx = 0;
        this->term_cidx = 0;
    
        if (this->chars != nullptr) {
            for (int r = 0; r < this->term_row; r ++) {
                this->clear_term_cell(r, 0, this->term_col);
            }
        }

        this->sync_cursor();
    }
        
private:
    void try_scroll_screen() {
        if (this->chars != nullptr) {
            for (int r = 0; r < this->term_row - 1; r ++) {
                for (int c = 0; c < this->term_col; c ++) {
                    this->clear_motion_actions(this->chars[r][c]);
                    this->chars[r][c]->set_text(MatterPort::CC, "%s", this->chars[r + 1][c]->c_str());

                    if (this->chars[r][c]->content_size() == 1) {
                        this->move_to_grid(this->chars[r][c], r + 1, c, MatterPort::CC);
                    } else {
                        this->move_to_grid(this->chars[r][c], r + 1, c, MatterPort::CC, { GRID_CELL_WIDTH * 0.5F, 0.0F });
                    }

                    this->glide(CHAR_DURATION, this->chars[r][c], { 0.0F, -GRID_CELL_HEIGHT });
                }
            }

            this->clear_term_cell(this->term_row - 1, 0, this->term_col);
        }
    }

    void clear_term_cell(int row, int col, int count) {
        int end_idx = col + count;

        if (end_idx > this->term_col) {
            end_idx = this->term_col;
        }

        for (int c = col; c < end_idx; c ++) {
            this->chars[row][c]->show(false);
            this->chars[row][c]->set_foreground_color(DIMGRAY);
        }
    }

private:
    Plteen::Labellet* cursor = nullptr;
    Plteen::Labellet*** chars = nullptr;

private:
    shared_font_t term_font;
    int term_row = 0;
    int term_col = 0;
    int term_ridx = 0;
    int term_cidx = 0;
};

/*************************************************************************************************/
static const double CMDLET_DURATION = 1.0;
static const double CURSOR_DURATION = 0.2;
static const double MODULE_DURATION = 2.0;
static const double CHAR_DURATION = 0.2;
static const double LASER_DURATION = 0.5;
static const float GRID_CELL_WIDTH = 32.0F;
static const float GRID_CELL_HEIGHT = 64.0F;
static const float LASER_THICKNESS = 4.0F;
static const int LASER_COL0 = -2;

static const RGBA true_color = RGBA(ROYALBLUE, 0.81);
static const RGBA mirror_color = RGBA(CHOCOLATE, 0.81);

/*************************************************************************************************/
Plteen::Consolet::Consolet(int r, int c, const char* name)
    : Consolet(GameFont::monospace(FontSize::x_large), r, c, name) {}

Plteen::Consolet::Consolet(shared_font_t font, int r, int c, const char* name)
    : Planelet(new GhostConsolePlane(font, r, c, name)) {}

/*************************************************************************************************/
void Plteen::Consolet::display(const std::string& message, bool sync) {
}

void Plteen::Consolet::linefeed(bool sync) {
    
}

void Plteen::Consolet::sync_cursor() {
    this->glide_to_grid(CURSOR_DURATION, this->cursor, this->term_ridx, this->term_cidx, MatterPort::CC);
}
