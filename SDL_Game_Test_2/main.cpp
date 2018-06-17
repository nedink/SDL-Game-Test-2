//
//  main.cpp
//  SDLGameTest
//
//  Created by David Bryant on 11/1/17.
//  Copyright © 2017 nedink. All rights reserved.
//

#include "Game.h"
#include <iostream>
#include <cstdio>
#include <vector>
#include <cstring>

Game* game = nullptr;

int main(int argc, const char * argv[]) {
    
    Uint32 frameStart;
    Uint32 frameTime;
    
    game = new Game();
    game->init();
    
    while (game->running()) {
        
        int frameDelay = 1000 / game->FPS;
        
        frameStart = SDL_GetTicks();
        
        game->handleEvents();
        game->step();
        game->render();
        
        frameTime = SDL_GetTicks() - frameStart;
        
        (frameDelay > frameTime) ? SDL_Delay(frameDelay - frameTime) : SDL_Delay(0);
    }
    
    game->clean();
    delete game;
    
    return 0;
}

