//
//  main.cpp
//  SDLGameTest
//
//  Created by David Bryant on 11/1/17.
//  Copyright © 2017 nedink. All rights reserved.
//

#include "Game.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <cstring>

Game* game = nullptr;

int main(int argc, const char * argv[]) {
    
    const int FPS = 120;
    const int frameDelay = 1000 / FPS;
    
    Uint32 frameStart;
    Uint32 frameTime;
    
    game = new Game();
    game->init("Game",
               SDL_WINDOWPOS_CENTERED,
               SDL_WINDOWPOS_CENTERED,
               TILESIZE * WINDOW_SIZE_TILES_WIDTH,
               TILESIZE * WINDOW_SIZE_TILES_HEIGHT,
               false);
    
    while (game->running()) {
        frameStart = SDL_GetTicks();
        
        game->handleEvents();
        game->step();
        game->render();
        
        frameTime = SDL_GetTicks() - frameStart;
        
        printf("%i\n", frameTime);
        
        (frameDelay > frameTime) ? SDL_Delay(frameDelay - frameTime) : SDL_Delay(0);
    }
    
    game->clean();
    delete game;
    
    return 0;
}

