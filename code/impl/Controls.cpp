#include "Controls.hpp"
#include <math/Math.hpp>
#include <appWindow.hpp>

void FFF::Controls::checkControls(LIA::KeybindingManager* keyBindingManager, LIA::Object* player, std::string name, float moveSpeed) {
    LIA::KeybindingControls &playerControls = keyBindingManager->getControls(name);
    if (LIA::AppWindow::isKeyPressed(name, playerControls.get("left"))) {
        player->_rotation.y = LIA::Math::toRadians(-90);
        player->_physics._force.x = -moveSpeed;
    } else if (LIA::AppWindow::isKeyPressed(name, playerControls.get("right"))) {
        player->_physics._force.x = +moveSpeed;
        player->_rotation.y = LIA::Math::toRadians(90);
    } else if (LIA::AppWindow::isKeyPressed(name, playerControls.get("forward"))) {
        player->_physics._force.z = -moveSpeed;
        player->_rotation.y = LIA::Math::toRadians(180);
    } else if (LIA::AppWindow::isKeyPressed(name, playerControls.get("backward"))) {
        player->_physics._force.z = +moveSpeed;
        player->_rotation.y = 0;
    }
}