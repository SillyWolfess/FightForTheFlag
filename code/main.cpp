#include <Engine.hpp>
#include "FightForTheFlag.hpp"

int main() {
    LIA::Engine& engine = LIA::Engine::getInstance();
    FFF::FightForTheFlag game;
    engine.setGame(&game);
    engine.run();
    return 0;
}