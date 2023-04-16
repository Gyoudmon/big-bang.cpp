#pragma once

#include "../agent.hpp"

namespace WarGrey::STEM {
    class Linkmon : public WarGrey::STEM::AgentSpriteSheet {
    public:
        Linkmon();

    public:
        void goodbye(int repeat = 1) override { this->play_goodbye(repeat); }
        void greetings(int repeat = 1) override { this->play_greeting(repeat); }
        
    public:
        void play_goodbye(int repeat = 1) { this->play("GoodBye", repeat); }
        void play_greeting(int repeat = 1) { this->play("Greeting", repeat); }
        void play_show(int repeat = 1) { this->play("Show", repeat); }
        void play_hide(int repeat = 1) { this->play("Hide", repeat); }
        
        void play_print(int repeat = 1) { this->play("Print", repeat); }
        void play_writing(int repeat = 1) { this->play("Writing", repeat); }
        void play_checking_something(int repeat = 1) { this->play("CheckingSomething", repeat); }
        void play_searching(int repeat = 1) { this->play("Searching", repeat); }
        void play_hearing(int repeat = 1) { this->play("Hearing", repeat); }
        void play_send_mail(int repeat = 1) { this->play("SendMail", repeat); }
        void play_save(int repeat = 1) { this->play("Save", repeat); }
        void play_empty_trash(int repeat = 1) { this->play("EmptyTrash", repeat); }
        void play_wave(int repeat = 1) { this->play("Wave", repeat); }
        void play_explain(int repeat = 1) { this->play("Explain", repeat); }
        void play_congratulate(int repeat = 1) { this->play("Congratulate", repeat); }
        void play_thinking(int repeat = 1) { this->play("Thinking", repeat); }
        void play_processing(int repeat = 1) { this->play("Processing", repeat); }
        void play_alert(int repeat = 1) { this->play("Alert", repeat); }
        
        void play_look_right(int repeat = 1) { this->play("LookRight", repeat); }
        void play_look_down(int repeat = 1) { this->play("LookDown", repeat); }
        void play_look_left(int repeat = 1) { this->play("LookLeft", repeat); }
        void play_look_down_right(int repeat = 1) { this->play("LookDownRight", repeat); }
        void play_look_up_right(int repeat = 1) { this->play("LookUpRight", repeat); }
        void play_look_down_left(int repeat = 1) { this->play("LookDownLeft", repeat); }
        void play_look_up(int repeat = 1) { this->play("LookUp", repeat); }
        void play_look_up_left(int repeat = 1) { this->play("LookUpLeft", repeat); }
        void play_gesture_up(int repeat = 1) { this->play("GestureUp", repeat); }
        void play_gesture_down(int repeat = 1) { this->play("GestureDown", repeat); }
        void play_gesture_right(int repeat = 1) { this->play("GestureRight", repeat); }
        void play_gesture_left(int repeat = 1) { this->play("GestureLeft", repeat); }

        void play_get_attention(int repeat = 1) { this->play("GetAttention", repeat); }
        void play_get_techy(int repeat = 1) { this->play("GetTechy", repeat); }
        void play_get_wizardy(int repeat = 1) { this->play("GetWizardy", repeat); }
        void play_get_artsy(int repeat = 1) { this->play("GetArtsy", repeat); }
        
        void play_idle_1(int repeat = 1) { this->play("Idle1", repeat); }
        void play_idle_leg_lick(int repeat = 1) { this->play("IdleLegLick", repeat); }
        void play_idle_tail_wag_A(int repeat = 1) { this->play("IdleTailWagA", repeat); }
        void play_idle_tail_wag_B(int repeat = 1) { this->play("IdleTailWagB", repeat); }
        void play_idle_tail_wag_C(int repeat = 1) { this->play("IdleTailWagC", repeat); }
        void play_idle_tail_wag_D(int repeat = 1) { this->play("IdleTailWagD", repeat); }
        void play_idle_yawn(int repeat = 1) { this->play("IdleYawn", repeat); }
        void play_idle_cleaning(int repeat = 1) { this->play("IdleCleaning", repeat); }
        void play_idle_blink(int repeat = 1) { this->play("IdleBlink", repeat); }
        void play_idle_twitch(int repeat = 1) { this->play("IdleTwitch", repeat); }
        void play_idle_stretch(int repeat = 1) { this->play("IdleStretch", repeat); }
        void play_idle_scratch(int repeat = 1) { this->play("IdleScratch", repeat); }
        void play_idle_butterfly(int repeat = 1) { this->play("IdleButterFly", repeat); }
        void play_deep_idle_A(int repeat = 1) { this->play("DeepIdleA", repeat); }
        void play_deep_idle_E(int repeat = 1) { this->play("DeepIdleE", repeat); }
        void play_rest_pose(int repeat = 1) { this->play("RestPose", repeat); }
        
    protected:
        int submit_idle_frames(std::vector<std::pair<int, int>>& frame_refs, int& times) override;
        int submit_action_frames(std::vector<std::pair<int, int>>& frame_refs, const std::string& action) override;
        int update_action_frames(std::vector<std::pair<int, int>>& frame_refs, int next_branch) override;

    private:
        int find_agent_frames_by_index(int frame_idx);
        int push_action_frames(std::vector<std::pair<int, int>>& frame_refs, const std::string& action, int idx0 = 0);
    };
}
