#include "logs.hpp"
#include "FightForTheFlag.hpp"

#include "gui/gui.hpp"
#include "manager/eventManager.hpp"
#include "manager/componentManager.hpp"
#include "manager/modelManager.hpp"
#include "manager/objectManager.hpp"
#include "data/Object.hpp"
#include "Engine.hpp"
#include "math/Math.hpp"

bool FFF::FightForTheFlag::init() {
    _objectId = -1;
    _eventSource = "fightForTheflag";
    return true;
}

bool FFF::FightForTheFlag::registerHandlers() {
    if (!subscribe("load")) {
        LIA_fatal("Failed to subscribe to load event");
        return false;
    }
    if (!subscribe("tick")) {
        LIA_fatal("Failed to subscribe to tick event");
        return false;
    }
    return true;
}

bool FFF::FightForTheFlag::onLoad(LIA::Event& event) {
    if (event.source.compare("game") != 0) {
        return false;
    }
    LIA::ObjectManager* objectManager = getObjectManager();
    LIA_trace("Registering data paths");
    if (!objectManager->registerPathsFromFile("./data/simulation/data.xml")) {
        LIA_fatal("Failed to register paths from file");
        LIA::Engine::getInstance().fatal();
        return true;
    }
    LIA_trace("Creating player1");
    if (!objectManager->createObject("player1", "player1")) {
        LIA_fatal("Failed to load player1");
        LIA::Engine::getInstance().fatal();
        return true;
    }
    _lastProjectileId = 0;
    _wasShootingPressed = false;
    return true;
}

bool FFF::FightForTheFlag::onTick(LIA::Event& event) {
    float rotSpeed = 0.1f;
    float moveSpeed = 0.2f;
    float projectileSpeed  = 0.5f;
    LIA::ObjectManager *objectManager = getObjectManager();
    _objectId = objectManager->getPlayer(_objectId);
    LIA::Object *player = objectManager->get(_objectId);
    
    player->_physics._force.x = 0;
    player->_physics._force.z = 0;
    if (LIA::AppWindow::isKeyPressed(_eventSource, 'A')) {
        player->_rotation.y = LIA::Math::toRadians(-90);
        player->_physics._force.x = -moveSpeed;
    } else if (LIA::AppWindow::isKeyPressed(_eventSource, 'D')) {
        player->_physics._force.x = +moveSpeed;
        player->_rotation.y = LIA::Math::toRadians(90);
    } else if (LIA::AppWindow::isKeyPressed(_eventSource, 'W')) {
        player->_physics._force.z = -moveSpeed;
        player->_rotation.y = LIA::Math::toRadians(180);
    } else if (LIA::AppWindow::isKeyPressed(_eventSource, 'S')) {
        player->_physics._force.z = +moveSpeed;
        player->_rotation.y = 0;
    }
    
    if (LIA::AppWindow::isKeyPressed(_eventSource, 'Q')) {
        if (!_wasShootingPressed) {
            std::string projectileName = std::vformat("projectile[{}]", std::make_format_args(_lastProjectileId));
            if (!objectManager->createObject("projectile", projectileName)) {
                LIA_fatal("Failed to load projectile");
                LIA::Engine::getInstance().fatal();
                return true;
            }
            _lastProjectileId++;
            int projectileId = objectManager->getByName(-1, projectileName);
            LIA::Object *projectile = objectManager->get(projectileId);
            LIA::copy(projectile->_position, player->_position);
            LIA::copy(projectile->_rotation, player->_rotation);
            LIA_debug_f("Projectile with identifier {} and id {} was created", projectileName, projectileId);
            _wasShootingPressed = true;
        }
    } else {
        _wasShootingPressed = false;
    }
    // movement
    player->_position.x = player->_position.x + player->_physics._force.x;
    player->_position.z = player->_position.z + player->_physics._force.z;
    return true;
}