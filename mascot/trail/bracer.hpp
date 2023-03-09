#pragma once

#include "citizen.hpp"

namespace WarGrey::STEM {
    class Bracer : public WarGrey::STEM::Citizen {
    public:
        Bracer(const char* name);
        virtual ~Bracer() {}

        void construct(SDL_Renderer* renderer) override;
    };

    class Estelle : public WarGrey::STEM::Bracer {
    public:
        Estelle() : Bracer("Estelle") {}

        virtual ~Estelle() {}
    };

    class Agate : public WarGrey::STEM::Bracer {
    public:
        Agate() : Bracer("Agate") {}
        virtual ~Agate() {}
    };
}
