#ifndef INSPECTOR_HPP
#define INSPECTOR_HPP
#include <component/BaseGame.hpp>
#include <string>
#include <map>
#include <data/position.hpp>
#include <data/Object.hpp>

#include "Collision.hpp"
#include "Controls.hpp"
#include "Projectiles.hpp"

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
            Collision _collision;
            Controls _controls;
            Projectiles _projectiles;

            // gui
            std::string _ingameBottomBar;
            // data
            int _objectId;
            int _materialId;
            int _winCondition;
            std::string _eventSource;
            
            bool _wasShootingPressed;
            bool _wasShootingPressedNpc;
            
            std::map<std::string, int> _npcs;
            
            // Score
            std::map<std::string, int> _hits;
            
            // Functions
            void updateScoreUi(std::string field, std::string owner);

            void checkWinLoss(std::string& playerName, std::string& npcName);
            void hideCustomIngameWindows();
    };
}
#endif