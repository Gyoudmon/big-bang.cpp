#include "bang.hpp"

using namespace WarGrey::STEM;

/*************************************************************************************************/
static std::string the_name = "青少计算机科学";

/*************************************************************************************************/
void WarGrey::STEM::the_big_bang_name(const char* name) {
    the_name = std::string(name);
}
    
const char* WarGrey::STEM::the_big_bang_name() {
    return the_name.c_str();
}

/*************************************************************************************************/
void WarGrey::STEM::TheBigBang::load(float width, float height) {
    this->title = this->insert(new Labellet(GameFont::Title(), this->title_color, "%s: %s", the_name.c_str(), this->name()));

    this->agent = this->insert(new Linkmon());
    this->agent->scale(-1.0F, 1.0F);

    this->title->camouflage(true);
    this->set_sentry_sprite(this->agent);
    this->move_to(this->title, this->agent, MatterAnchor::RB, MatterAnchor::LB);

    this->tooltip = this->insert(make_label_for_tooltip(GameFont::Tooltip()));
    this->set_tooltip_matter(this->tooltip);
}

float WarGrey::STEM::TheBigBang::get_titlebar_height() {
    float height = 0.0F;

    if (this->agent != nullptr) {
        this->agent->feed_extent(0.0F, 0.0F, nullptr, &height);
    }

    return height;
}
