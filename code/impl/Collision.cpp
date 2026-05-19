#include "Collision.hpp"
#include <math/Math.hpp>

bool FFF::Collision::isColliding(LIA::Object* target, LIA::Object* source) {
    return isColliding(target, source->_position, source->_scale) || isColliding(source, target->_position, target->_scale);
}

bool FFF::Collision::isColliding(LIA::Object* target, LIA::Position newPosition, LIA::Scale scale) {
    const LIA::Position targetPos = target->_position;
    const LIA::Scale targetScale = target->_scale;
        
    bool xLeft = LIA::Math::isInBounds(newPosition.x - scale.x, targetPos.x, targetScale.x);
    bool xRight = LIA::Math::isInBounds(newPosition.x + scale.x, targetPos.x, targetScale.x);
    bool yUp = LIA::Math::isInBounds(newPosition.z - scale.z, targetPos.z, targetScale.z);
    bool yDown = LIA::Math::isInBounds(newPosition.z + scale.z, targetPos.z, targetScale.z);
/*
    LIA_trace_f("target {}: [{} x {}] , [{} x {}]", target->_name, targetPos.x, targetPos.z, targetScale.x, targetScale.z);
    LIA_trace_f("position, scale: [{} x {}], [{} x {}]", newPosition.x, newPosition.z, scale.x, scale.z);
    LIA_trace_f("collision {}, {}, {}, {}", xLeft, xRight, yUp, yDown);
*/
    if (!xLeft && !xRight && !yUp && !yDown) {
        return false;
    }
    if (yUp || yDown) {
        if (!xLeft && !xRight) {
              return false;
        }     
        return true;
    }
    if (xLeft || xRight) {
        if (!yUp && !yDown) {
               return false;
        }
        return true;
    }
    return false;
}