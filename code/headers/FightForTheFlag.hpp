#ifndef INSPECTOR_HPP
#define INSPECTOR_HPP
#include <component/BaseGame.hpp>
#include <string>

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
            int _objectId;
            int _materialId;
            int _lastProjectileId;
            std::string _eventSource;
            bool _wasShootingPressed;
    };
}
#endif