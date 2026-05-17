#ifndef INSPECTOR_HPP
#define INSPECTOR_HPP
#include <component/BaseGame.hpp>
#include <string>
#include <map>
#include <data/position.hpp>
#include <data/Object.hpp>

namespace FFF {
    class FightForTheFlag : public LIA::BaseGame {
        public:
//            virtual bool afterInit();
        protected:
            bool init();
            virtual bool registerHandlers();
            virtual bool onLoad(LIA::Event&);
            virtual bool onTick(LIA::Event&);
        private:
            // gui
            std::string _ingameBottomBar;
            // data
            int _objectId;
            int _materialId;
            int _lastProjectileId;
            std::string _eventSource;
            
            bool _wasShootingPressed;
            bool _wasShootingPressedNpc;
            
            std::map<std::string, int> _projectiles;
            std::map<std::string, int> _npcs;
            std::map<std::string, std::string> _projectileOwner;
            
            // Score
            std::map<std::string, int> _hits;
            
            // Functions
            bool isColliding(LIA::Object* target, LIA::Position newPosition, LIA::Scale scale);
            bool isColliding(LIA::Object* target, LIA::Object* source);
            void updateScoreUi(std::string field, std::string owner);
    };
}
#endif