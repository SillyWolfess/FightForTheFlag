#include "Projectiles.hpp"

#include <format>
#include <Engine.hpp>

void FFF::Projectiles::move(LIA::ObjectManager *objectManager) {
    for (auto [name, id] : _data) {
        LIA::Object *projectile = objectManager->get(id);
        projectile->_position.x = projectile->_position.x + projectile->_physics._force.x;
        projectile->_position.z = projectile->_position.z + projectile->_physics._force.z;
    }
}

bool FFF::Projectiles::spawn(LIA::ObjectManager *objectManager, LIA::Object* player, std::string playerNameId) {
    std::string projectileName = std::vformat("projectile[{}]", std::make_format_args(_lastProjectileId));
    if (!objectManager->createObject("projectile", projectileName)) {
        LIA_fatal("Failed to load projectile");
        LIA::Engine::getInstance().fatal();
        return false;
    }
    _lastProjectileId++;
    int projectileId = objectManager->getByName(-1, projectileName);
    LIA::Object *projectile = objectManager->get(projectileId);
    LIA::copy(projectile->_position, player->_position);
    LIA::copy(projectile->_rotation, player->_rotation);
            
    LIA::applyForceByAngleXZ(projectile->_physics._force, projectile->_rotation.y, _projectileSpeed);

    _data.emplace(std::pair<std::string, int>(projectileName, projectileId));
    _owners.emplace(std::pair<std::string, std::string> (projectileName, playerNameId));
    LIA_debug_f("Projectile [name = {} ,id = {}, owner = {}] was created", projectileName, projectileId, _owners[projectileName]);
    return true;
}

void FFF::Projectiles::cleanup(LIA::ObjectManager *objectManager) {
    std::vector<std::string> toRemove;
    for (auto [name, id] : _data) {
        int newId = objectManager->getByName(id, name);
        if (newId == -1) {
            toRemove.push_back(name);
            continue;
        }
        if (id != newId) {
            _data[name] = newId;
        }
    }
    for (std::string name: toRemove) {
        _data.erase(name);
    }
}

void FFF::Projectiles::clear() {
    _lastProjectileId = -1;
    _data.clear();
    _owners.clear();
}

std::string FFF::Projectiles::getOwner(std::string id) {
    return _owners[id];
}