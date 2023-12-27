#pragma once

#include <string>

namespace GYDM {
    enum class CreatureGender { Male, Female, _ };

    class __lambda__ ICreature {
    public:
        void give_nickname(const char* name) { this->_nickname = (name == nullptr) ? "" : name; }
        void give_nickname(const std::string& name) { this->_nickname = name; }

    public:
        virtual const char* nickname() { return this->_nickname.c_str(); }
        virtual CreatureGender gender() { return CreatureGender::_; }

    private:
        std::string _nickname;
    };
}
