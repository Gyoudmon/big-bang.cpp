#pragma once

#include "../agent.hpp"

namespace WarGrey::STEM {
    class Linkmon : public WarGrey::STEM::AgentSpriteSheet {
    public:
        Linkmon();

    public:
        void greetings(int repeat = 1) override { this->play("Greeting", repeat); } 
        void goodbye(int repeat = 1) override { this->play("GoodBye", repeat); }

    protected:
        int submit_idle_frames(std::vector<std::pair<int, int>>& frame_refs, int& times) override;
        int submit_action_frames(std::vector<std::pair<int, int>>& frame_refs, const std::string& action) override;
        int update_action_frames(std::vector<std::pair<int, int>>& frame_refs, int next_branch) override;

    private:
        int find_agent_frames_by_index(int frame_idx);
        int push_action_frames(std::vector<std::pair<int, int>>& frame_refs, const std::string& action, int idx0 = 0);
    };
}
