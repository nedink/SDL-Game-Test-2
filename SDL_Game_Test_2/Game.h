
#pragma once

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_gfx/SDL2_gfxPrimitives.h>

#include <stdio.h>
#include <vector>

#include "values.h"

#include "player.h"
#include "camera.h"

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

class Player;

class Game {
public:
    bool isRunning;
    
    Game();
    Game(const Game &game);
    ~Game();
    
    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscrean);
    
    void handleEvents();
    void step();
    bool running() { return isRunning; }
    void render();
    void clean();
    
    xy mapTileXY(xy xy);
    int mapTile(xy xy);
    
    bool grounded(aabb aabb);
    int groundedTile(xy xy);
    
    static void AddTile(int id, int x, int y);
    static SDL_Renderer* renderer;
    static SDL_Event event;
    
    std::vector<bool> keysDown = {false, false, false, false, false, false};
    
    Game* prevFrame = nullptr;
    bool mouseDown = false;
    bool jumped = false;
    xy mouse;
    line mouseIndicator;
    Player* player;
    Camera* camera;
    u_long ticks = 0;
    
//    float timeStep = 1.0f / 60.0f;
    int velocityIterations = 6;
    int positionIterations = 2;
    
    int map;
    
private:
    int count = 0;
    SDL_Window *window;
};

enum keys {
    W, A, S, D,
    Q, E,
    UP, DOWN, LEFT, RIGHT,
    SPACE,
};
