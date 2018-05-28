#include "player.h"
#include "Game.h"

#include <stdio.h>
#include <math.h>

using namespace util;
using namespace std;

Player::Player(Game* game_)
{
    game = game_;
}

Player::Player(const Player &player)
{
//    game = new Game(*player.game);
}

Player::~Player()
{}

void Player::setWidth(float w)
{
    body.w = w;
}

void Player::setHeight(float h)
{
    body.h = h;
}

void Player::setPos(xy xy)
{
    pos = xy;
    body = pos;
}

void Player::setVel(xy xy) {
    vel = xy;
}

aabb Player::getBody() {
    return body;
}

xy Player::getPos() {
    return pos;
}

xy Player::getVel() {
    return vel;
}


void Player::doWeakForces()
{
    if (game->keysDown[D])
    {
        applyForce({(float)(ACC_GROUND + (vel.x < 0 ? DE_ACC_GROUND : 0)), 0});
    }
    else if (game->keysDown[A]) // force left
    {
        applyForce({(float)(-ACC_GROUND - (vel.x > 0 ? DE_ACC_GROUND : 0)), 0});
    }
    else {
        if (vel.x < 0)
        {
            if (game->grounded(body))
                applyForce({DE_ACC_GROUND, 0});
            else
                applyForce({DE_ACC_AIR, 0});
            if (vel.x > 0)
                setVel({0, vel.y});
        }
        else if (vel.x > 0) {
            if (game->grounded(body))
                applyForce({-DE_ACC_GROUND, 0});
            else
                applyForce({-DE_ACC_AIR, 0});
            if (vel.x < 0)
                setVel({0, vel.y});
        }
    }
    
    // jump
    if (game->grounded(body)) {
        //        printf("jumped: %i\n", jumped);
        if (game->keysDown[W]) {
            if (!jumped) {
                applyForce({0, -JUMP_VEL});
                jumped = true;
            }
        }
    }
}

void Player::applyForce(xy xy) {
    setVel(vel + xy);
}

void Player::displace(xy xy) {
    setPos(pos + xy);
}

void Player::moveTo(xy xy) {
    setPos(xy);
}
