#pragma once

#include "physics.h"

class Game;

class Player : public phys::PhysObj {

private:
    Game* game;
    
public:
    Player(Game* game_);
    Player(const Player &player);
    ~Player();
    
    bool injured = false;
    bool jumped = false;
    
    void doWeakForces();
};

