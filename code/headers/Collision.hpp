#ifndef COLLISION_HPP
#define COLLISION_HPP
#include <data/position.hpp>
#include <data/Object.hpp>

namespace FFF {
    class Collision {
        public:
            bool isColliding(LIA::Object* target, LIA::Position newPosition, LIA::Scale scale);
            bool isColliding(LIA::Object* target, LIA::Object* source);
    };
}
#endif