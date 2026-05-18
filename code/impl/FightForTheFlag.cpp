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
#include "tools.hpp"

bool FFF::FightForTheFlag::init() {
    _objectId = -1;
    _eventSource = "fightForTheflag";
    _ingameBottomBar = "ingameBottomBar";
    LIA_TRY
        if (!getGuiManager()->loadWindow(_ingameBottomBar, "./data/fff/gui/data/ingameBottomBar.xml")) {
            LIA_error_f("Failed to init window for {}", _ingameBottomBar);
            return false;
        }
    LIA_CATCH_RETURN_FALSE
    LIA_TRY
        if (!getGuiManager()->loadWindow("ffLostWindow", "./data/fff/gui/data/ffLostWindow.xml")) {
            LIA_error("Failed to init ffLostWindow");
            return false;
        }
        if (!getGuiManager()->loadWindow("ffWonWindow", "./data/fff/gui/data/ffWonWindow.xml")) {
            LIA_error("Failed to init ffWonWindow");
            return false;
        }
    LIA_CATCH_RETURN_FALSE
    LIA_TRY
        if (!getKeybindingManager()->registerControls("player", "./data/settings/controls/player.xml", true)) {
            LIA_fatal("Failed to register controls for player");
            return false;
        }
        if (!getKeybindingManager()->load("player")) {
            LIA_fatal("Failed to load player controls");
            return false;
        }
        if (!getKeybindingManager()->registerControls("npc", "./data/settings/controls/npc.xml", true)) {
            LIA_fatal("Failed to register controls for npc");
            return false;
        }
        if (!getKeybindingManager()->load("npc")) {
            LIA_fatal("Failed to load npc controls");
            return false;
        }
    LIA_CATCH_RETURN_FALSE
    return true;
}

bool FFF::FightForTheFlag::registerHandlers() {
    if (!subscribe(LIA::ComponentEvent::LOAD)) {
        LIA_fatal("Failed to subscribe to load event");
        return false;
    }
    if (!subscribe(LIA::ComponentEvent::TICK)) {
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
    LIA_trace("Create npc");
    if (!objectManager->createObject("npc", "npc[1]")) {
        LIA_fatal("Failed t load npc[1]");
        LIA::Engine::getInstance().fatal();
        return true;
    }
    int npc1Id = objectManager->getByName(-1, "npc[1]");
    _npcs.clear();
    _npcs.emplace(std::pair<std::string, int>("npc[1]", npc1Id));

    _hits.clear();
    _hits.emplace(std::pair<std::string, int>("npc[1]", 0));
    _hits.emplace(std::pair<std::string, int>("player1", 0));
    
    LIA::Object *npc1 = objectManager->get(npc1Id);
    npc1->_position.x = 50;
    npc1->_position.y = 0;

    _projectiles.clear();
    _projectileOwner.clear();
     _lastProjectileId = 0;
    _wasShootingPressed = false;
    _wasShootingPressedNpc = false;

    updateScoreUi("hits[player]", "player1");
    updateScoreUi("hits[npc]", "npc1");
    getGuiManager()->openWindow(_ingameBottomBar);
    return true;
}

bool FFF::FightForTheFlag::onTick(LIA::Event& event) {
    float rotSpeed = 0.1f;
    float moveSpeed = 0.2f;
    float projectileSpeed  = 0.5f;
    LIA::ObjectManager *objectManager = getObjectManager();
    std::string playerNameId = "player1";
    _objectId = objectManager->getPlayer(_objectId);
    LIA::Object *player = objectManager->get(_objectId);
    
    player->_physics._force.x = 0;
    player->_physics._force.z = 0;
    
    LIA::KeybindingManager *keyBindingManager = getKeybindingManager();
    LIA::KeybindingControls &playerControls = keyBindingManager->getControls("player");

    if (LIA::AppWindow::isKeyPressed("player", playerControls.get("left"))) {
        player->_rotation.y = LIA::Math::toRadians(-90);
        player->_physics._force.x = -moveSpeed;
    } else if (LIA::AppWindow::isKeyPressed("player", playerControls.get("right"))) {
        player->_physics._force.x = +moveSpeed;
        player->_rotation.y = LIA::Math::toRadians(90);
    } else if (LIA::AppWindow::isKeyPressed("player", playerControls.get("forward"))) {
        player->_physics._force.z = -moveSpeed;
        player->_rotation.y = LIA::Math::toRadians(180);
    } else if (LIA::AppWindow::isKeyPressed("player", playerControls.get("backward"))) {
        player->_physics._force.z = +moveSpeed;
        player->_rotation.y = 0;
    }
    
    if (LIA::AppWindow::isKeyPressed("player", playerControls.get("fire"))) {
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
            
            LIA::applyForceByAngleXZ(projectile->_physics._force, projectile->_rotation.y, projectileSpeed);

            _projectiles.emplace(std::pair<std::string, int>(projectileName, projectileId));
            _projectileOwner.emplace(std::pair<std::string, std::string> (projectileName, playerNameId));
            LIA_debug_f("Projectile [name = {} ,id = {}, owner = {}] was created", projectileName, projectileId, _projectileOwner[projectileName]);
            _wasShootingPressed = true;
        }
    } else {
        _wasShootingPressed = false;
    }
    // movement
    LIA::Position prevPos = LIA::emptyPosition();
    LIA::copy(prevPos, player->_position);

    player->_position.x = player->_position.x + player->_physics._force.x;
    player->_position.z = player->_position.z + player->_physics._force.z;
    
    float respawnRadius = 50;
    float borderRadius = respawnRadius + 20;
    bool playerCollided = false;
    std::map<std::string, bool> npcCollidedMap;
    std::map<std::string, LIA::Position> npcPrevPos;

    // copy npc pos before moving them
    for (auto [name, id]: _npcs) {
        id = objectManager->getByName(id, name);
        if (id == -1) {
            continue;
        }
        npcCollidedMap.emplace(std::pair<std::string, bool>(name, false));
        npcPrevPos.emplace(std::pair<std::string, LIA::Position>(name, objectManager->get(id)->_position));
    }

    // Two player game, second player controls npc
    std::string pNpcName;
    int pNpcId = -1;
    for (auto [name, id]: _npcs) {
        pNpcName = name;
        pNpcId = id;
        break;
    }
    LIA::KeybindingControls &npcControlls = keyBindingManager->getControls("npc");
    pNpcId = objectManager->getByName(pNpcId, pNpcName);
    LIA::Object *pNpc = objectManager->get(pNpcId);

    pNpc->_physics._force.x = 0;
    pNpc->_physics._force.z = 0;

    if (LIA::AppWindow::isKeyPressed("npc", npcControlls.get("left"))) {
        pNpc->_rotation.y = LIA::Math::toRadians(-90);
        pNpc->_physics._force.x = -moveSpeed;
    } else if (LIA::AppWindow::isKeyPressed("npc", npcControlls.get("right"))) {
        pNpc->_physics._force.x = +moveSpeed;
        pNpc->_rotation.y = LIA::Math::toRadians(90);
    } else if (LIA::AppWindow::isKeyPressed("npc", npcControlls.get("forward"))) {
        pNpc->_physics._force.z = -moveSpeed;
        pNpc->_rotation.y = LIA::Math::toRadians(180);
    } else if (LIA::AppWindow::isKeyPressed("npc", npcControlls.get("backward"))) {
        pNpc->_physics._force.z = +moveSpeed;
        pNpc->_rotation.y = 0;
    }

    if (LIA::AppWindow::isKeyPressed("npc", npcControlls.get("fire"))) {
        if (!_wasShootingPressedNpc) {
            std::string projectileName = std::vformat("projectile[{}]", std::make_format_args(_lastProjectileId));
            if (!objectManager->createObject("projectile", projectileName)) {
                LIA_fatal("Failed to load projectile");
                LIA::Engine::getInstance().fatal();
                return true;
            }
            _lastProjectileId++;
            int projectileId = objectManager->getByName(-1, projectileName);
            LIA::Object *projectile = objectManager->get(projectileId);
            LIA::copy(projectile->_position, pNpc->_position);
            LIA::copy(projectile->_rotation, pNpc->_rotation);
            
            LIA::applyForceByAngleXZ(projectile->_physics._force, projectile->_rotation.y, projectileSpeed);

            _projectiles.emplace(std::pair<std::string, int>(projectileName, projectileId));
            _projectileOwner.emplace(std::pair<std::string, std::string> (projectileName, pNpcName));
            LIA_debug_f("Projectile [name = {} ,id = {}, owner = {}] was created", projectileName, projectileId, _projectileOwner[projectileName]);
            _wasShootingPressedNpc = true;
        }
    } else {
        _wasShootingPressedNpc = false;
    }

    // move npc and check collision with bounds
    for (auto [name, id]: _npcs) {
        id = objectManager->getByName(id, name);
        if (id == -1) {
            continue;
        }
        LIA::Object* npc = objectManager->get(id);
        npc->_position.x = npc->_position.x + npc->_physics._force.x;
        npc->_position.z = npc->_position.z + npc->_physics._force.z;

        if (
            npc->_position.x < -borderRadius || npc->_position.x > borderRadius ||
            npc->_position.z < -borderRadius || npc->_position.z > borderRadius
        ) {
            LIA::copy(npc->_position, npcPrevPos[name]);
            npcCollidedMap[name] = true;
        }
    }

    // player collision with bounds
    if (
        player->_position.x < -borderRadius || player->_position.x > borderRadius ||
        player->_position.z < -borderRadius || player->_position.z > borderRadius
    ) {
        LIA::copy(player->_position, prevPos);
        playerCollided = true;
    }

    // player npc collision
    for (auto [name, id]: _npcs) {
        id = objectManager->getByName(id, name);
        if (id == -1) {
            continue;
        }
        LIA::Object *npc = objectManager->get(id);
        if (isColliding(player, npc)) {
            if (!playerCollided) {
                LIA::copy(player->_position, prevPos);
                playerCollided = true;
            }
            if (!npcCollidedMap[name]) {
                LIA::copy(npc->_position, npcPrevPos[name]);
                npcCollidedMap[name] = true;
            }
        }
    }

    std::vector<std::string> toRemove;
    // remove dead projectiles from map
    for (auto [name, id] : _projectiles) {
        int newId = objectManager->getByName(id, name);
        if (newId == -1) {
            toRemove.push_back(name);
            continue;
        }
        if (id != newId) {
            _projectiles[name] = newId;
        }
    }
    for (std::string name: toRemove) {
        _projectiles.erase(name);
    }
    // move projectiles
    for (auto [name, id] : _projectiles) {
        LIA::Object *projectile = objectManager->get(id);
        projectile->_position.x = projectile->_position.x + projectile->_physics._force.x;
        projectile->_position.z = projectile->_position.z + projectile->_physics._force.z;
    }

    std::vector<std::string> toDeleteObjects;
    std::vector<std::string> hitNpcs;
    std::map<std::string, bool> wasHit;
    bool hitPlayer = false;
    // projectile collision
    for (auto [name, id]: _projectiles) {
        LIA::Object *projectile = objectManager->get(id);
        // Check projectile is out of bounds
        if (
            projectile->_position.x < - respawnRadius * 2.0 || projectile->_position.x > respawnRadius * 2.0 ||
            projectile->_position.z < - respawnRadius * 2.0 || projectile->_position.z > respawnRadius * 2.0 
        ) {
            toDeleteObjects.push_back(name);
            continue;
        }
        // collide with player
        if (!hitPlayer && !LIA::equals(playerNameId, _projectileOwner[name])) {
            if (isColliding(projectile, player)) {
                LIA_debug_f("{} is colliding with {} [projectile owner = {}]", name, playerNameId, _projectileOwner[name]);
                // remove projectile
                toDeleteObjects.push_back(name);
                // hit player
                _hits[_projectileOwner[name]]++;
                hitPlayer = true;
                continue;
            }
        }
        // collide with npcs
        for (auto [npcName, npcId]: _npcs) {
            if (objectManager->getByName(npcId, npcName) == -1) {
                continue;
            }
            std::string sName = npcName;
            if (wasHit[npcName] || LIA::equals(sName, _projectileOwner[name])) {
                continue;
            }
            LIA::Object *npc = objectManager->get(npcId);
            if (isColliding(projectile, npc)) {
                LIA_debug_f("{} is colliding with {} [projectile owner = {}]", name, npcName, _projectileOwner[name]);
                // remove projectile
                toDeleteObjects.push_back(name);
                // hit npcs
                hitNpcs.push_back(npcName);
                _hits[_projectileOwner[name]]++;
                wasHit.emplace(std::pair<std::string, bool>(npcName, true));
                break;
            }
        }
    }
    // respawn player
    if (hitPlayer) {
        do {
            player->_position.x = LIA::Math::getRandomInt(-respawnRadius, respawnRadius);
            player->_position.z = LIA::Math::getRandomInt(-respawnRadius, respawnRadius);
        } while (LIA::Math::isColliding(player, pNpc)); // TODO check collision with every object
    }
    // respawn hit npcs
    for (auto name: hitNpcs) {
        int id = _npcs[name];
        if (objectManager->getByName(id, name) == -1) {
            continue;
        }
        LIA::Object *npc = objectManager->get(id);
        do {
            npc->_position.x = LIA::Math::getRandomInt(-respawnRadius, respawnRadius);
            npc->_position.z = LIA::Math::getRandomInt(-respawnRadius, respawnRadius);
        } while (LIA::Math::isColliding(player, npc)); // TODO check collision with every object
    }
    // tell Engine to clean up
    objectManager->remove(toDeleteObjects);
    // update gui
    updateScoreUi("hits[player]", playerNameId);
    updateScoreUi("hits[npc]", pNpcName);
    checkWinLoss(playerNameId, pNpcName);
    return true;
}

void FFF::FightForTheFlag::hideCustomIngameWindows() {
    getGuiManager()->closeWindow(_ingameBottomBar);
}

void FFF::FightForTheFlag::checkWinLoss(std::string &playerName, std::string &npcName) {
    if (_hits[playerName] >= _winCondition) {
        LIA_info("game won");
        LIA::GameWonEvent gameWonEvent("ffWonWindow");
        getEventManager()->handleEvent(gameWonEvent);
        hideCustomIngameWindows();
        return; 
    }
    if (_hits[npcName] >= _winCondition) {
        LIA_info("game lost");
        LIA::GameLostEvent gameLostEvent("ffLostWindow");
        getEventManager()->handleEvent(gameLostEvent);
        hideCustomIngameWindows();
        return;
    }
}

void FFF::FightForTheFlag::updateScoreUi(std::string field, std::string owner) {
    std::string hitsString = std::vformat("{}", std::make_format_args(_hits[owner]));
    LIA::UpdateGuiStringEvent scoreStringEvent(_ingameBottomBar, field, hitsString);
    getEventManager()->handleEvent(scoreStringEvent);
}

bool FFF::FightForTheFlag::isColliding(LIA::Object* target, LIA::Object* source) {
    return isColliding(target, source->_position, source->_scale) || isColliding(source, target->_position, target->_scale);
}

bool FFF::FightForTheFlag::isColliding(LIA::Object* target, LIA::Position newPosition, LIA::Scale scale) {
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