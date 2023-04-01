#pragma once

#include "../sheet.hpp"
#include "../../../datum/path.hpp"

#include <optional>
#include <vector>
#include <map>

/* MS Office Assistant, Link (the cat) */
/* https://github.com/jaseg/clippy */

namespace WarGrey::STEM {
    class AgentSpriteSheet : public WarGrey::STEM::SpriteGridSheet {
    public:
        AgentSpriteSheet(const std::string& pathname, int row, int col)
            : SpriteGridSheet(pathname, row, col) {}

    public:
        int preferred_local_fps() override { return 0; }

    protected:
        int get_initial_costume_index() override { return -1; }
        const char* costume_index_to_name(size_t idx) override { return "[THIS FUNCTION IS USELESS]"; }
    };

    /*********************************************************************************************/
    struct AgentFrameBranch {
        int frame_idx;
        int weight;
    };

    struct AgentAction {
        int duration;
        std::vector<std::pair<int, int>> images;
        std::vector<AgentFrameBranch> branches;
        std::optional<int> exit_branch;
        std::optional<std::string> sound;
    };

    struct AgentInfo {
        std::map<std::string, const std::vector<AgentAction>> frames;
        int width;
        int height;
        int layers;
        std::vector<std::string> sounds;
    };
}
