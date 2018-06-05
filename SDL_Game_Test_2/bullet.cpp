//
//  bullet.cpp
//  SDL_Game_Test_2
//
//  Created by David Bryant on 6/1/18.
//  Copyright © 2018 nedink. All rights reserved.
//

#include "bullet.h"

#include <stdio.h>

Bullet::Bullet()
{}

Bullet::~Bullet()
{}

void Bullet::update()
{
    pos += vel;
}
