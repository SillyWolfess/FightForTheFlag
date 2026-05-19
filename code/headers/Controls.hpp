#ifndef CONTROLS_HPP
#define CONTROLS_HPP
#include <data/position.hpp>
#include <data/Object.hpp>
#include <manager/keybindingManager.hpp>

namespace FFF {
    class Controls {
        public:
            void checkControls(LIA::KeybindingManager* keybinds, LIA::Object* player, std::string name, float moveSpeed); 
    };
}
#endif