#include "bang.hpp"

using namespace GYDM;

/*************************************************************************************************/
void GYDM::TheBigBang::load(float width, float height) {
    this->title = this->insert(new Labellet(GameFont::Title(), this->title_color,
        "%s: %s", this->_the_name.c_str(), this->name()));

    this->agent = this->insert(new Linkmon());
    this->agent->scale(-1.0F, 1.0F);

    this->title->camouflage(true);
    this->set_sentry_sprite(this->agent);
    this->move_to(this->title, Position(this->agent, MatterPort::RB), MatterPort::LB);

    this->tooltip = this->insert(make_label_for_tooltip(GameFont::Tooltip()));
    this->set_tooltip_matter(this->tooltip);
}

float GYDM::TheBigBang::get_titlebar_height() {
    float height = 0.0F;

    if (this->agent != nullptr) {
        height = this->agent->get_bounding_box().height();
    }

    return height;
}
