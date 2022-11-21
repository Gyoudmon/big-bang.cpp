#pragma once

namespace WarGrey::STEM {
    class IPlanet;
    class IHeadUpPlanet;
    class IPlanetDecorator;

    class IMatter;
    class ISprite;

    enum class MatterAnchor { LT, CT, RT, LC, CC, RC, LB, CB, RB };
    enum class BorderEdge { TOP, RIGHT, BOTTOM, LEFT, NONE };
    enum class BorderStrategy { IGNORE, STOP, BOUNCE };
}

