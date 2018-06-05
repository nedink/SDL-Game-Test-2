
#pragma once

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_gfx/SDL2_gfxPrimitives.h>

#include <stdio.h>
#include <vector>
#include <map>

#include "values.h"
#include "physics.h"

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

class Player;
class Camera;
class Bullet;

enum keys {
    W, A, S, D,
    Q, E,
    UP, DOWN, LEFT, RIGHT,
    SPACE,
};

class Game {
public:
    bool isRunning;
    
    Game();
    Game(const Game &game);
    ~Game();
    
    void init();
    
    void handleEvents();
    void step();
    bool running() { return isRunning; }
    void render();
    void clean();
    
    geom::xy getScreenXY(geom::xy xy);
    int getScreenX(int arg);
    int getScreenY(int arg);
    geom::xy getMapTileXY(geom::xy xy);
    int getMapTileVal(geom::xy xy);
    bool isGrounded(geom::aabb aabb);
    int groundedTile(geom::xy xy);
    
    static void AddTile(int id, int x, int y);
    static SDL_Renderer* renderer;
    static SDL_Event event;
    
    std::map<keys, bool> keysDown;
    
    int FPS = DEFAULT_FPS;
    
    Game* prevFrame = nullptr;
    bool mouseDown = false;
    bool jumped = false;
    geom::xy mouse;
    geom::line mouseIndicator;
    Player* player;
    std::vector<Bullet*> bullets;
    Camera* camera;
    u_long ticks = 0;
    int reloadTicks = 0;
    
    bool DEV_MODE;
    
//    float timeStep = 1.0f / 60.0f;
    int velocityIterations = 6;
    int positionIterations = 2;
    
    int map;
    
private:
    int count = 0;
    SDL_Window *window;
};
