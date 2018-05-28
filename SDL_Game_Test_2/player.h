
#pragma once

#include "util.h"
#include "Game.h"

using namespace util;

class Game;

class Player {

private:
    Game* game;
    
    aabb body;
    xy pos;
    xy vel;
    
public:
    Player(Game* game_);
    Player(const Player &player);
    ~Player();
    
    bool injured = false;
    
    void setWidth(float w);
    void setHeight(float h);
    void setPos(xy xy);
    void setVel(xy xy);
    
    void applyForce(xy xy);
    void displace(xy xy);
    void moveTo(xy xy);
    
    xy getPos();
    xy getVel();
	aabb getBody();
    
    void doWeakForces();
};

