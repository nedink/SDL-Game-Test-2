#include "player.h"
#include "Game.h"

#include <stdio.h>
#include <math.h>

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
            if (game->isGrounded(body))
                applyForce({DE_ACC_GROUND, 0});
            else
                applyForce({DE_ACC_AIR, 0});
            if (vel.x > 0)
                setVel({0, vel.y});
        }
        else if (vel.x > 0) {
            if (game->isGrounded(body))
                applyForce({-DE_ACC_GROUND, 0});
            else
                applyForce({-DE_ACC_AIR, 0});
            if (vel.x < 0)
                setVel({0, vel.y});
        }
    }
    
    // jump
    if (game->isGrounded(body)) {
//        if (game->keysDown[W]) {
        if (game->keysDown[W]) {
            if (!jumped) {
                applyForce({0, -JUMP_VEL});
                jumped = true;
            }
        }
    }
    if (!game->keysDown[W])
        jumped = false;
}
