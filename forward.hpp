#pragma once

namespace WarGrey::STEM {
    class IPlanet;
    class IHeadUpPlanet;
    class IPlanetDecorator;

    class ISprite;
    class IGraphlet;

    enum class GraphletAnchor { LT, CT, RT, LC, CC, RC, LB, CB, RB };
    enum class BorderEdge { TOP, RIGHT, BOTTOM, LEFT, NONE };
    enum class BorderCollisionStrategy { IGNORE, STOP, BOUNCE };
}

