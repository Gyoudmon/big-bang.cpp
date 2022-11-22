#pragma once

namespace WarGrey::STEM {
    class IPlane;
    class IMatter;

    enum class MatterAnchor { LT, CT, RT, LC, CC, RC, LB, CB, RB };
    enum class BorderEdge { TOP, RIGHT, BOTTOM, LEFT, NONE };
    enum class BorderStrategy { IGNORE, STOP, BOUNCE };
}

