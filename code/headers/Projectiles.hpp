#ifndef PROJECTILES_HPP
#define PROJECTILES_HPP

#include <map>
#include <string>
#include <manager/objectManager.hpp>
#include <data/Object.hpp>

namespace FFF {
    class Projectiles {
        private:
            int _lastProjectileId;
            float _projectileSpeed;
            std::map<std::string, int> _data;
            std::map<std::string, std::string> _owners;
        public:
            void clear();
            void setSpeed(float speed) { _projectileSpeed = speed; }
            void cleanup(LIA::ObjectManager *objectManager);
            void move(LIA::ObjectManager *objectManager);
            bool spawn(LIA::ObjectManager *objectManager, LIA::Object* player, std::string playerNameId);

            std::map<std::string, int>& getData() { return _data; }
            std::string getOwner(std::string id);
    };
}
#endif