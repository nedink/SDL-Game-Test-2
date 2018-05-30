#include <stdio.h>
#include <math.h>

#include "Game.h"
#include "maps.h"

using namespace std;

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;

Game::Game()
{}

Game::Game(const Game &game)
{
    this->player = new Player(*player);
    this->camera = new Camera(*camera);
}

Game::~Game()
{}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscrean)
{
    int flags = 0;
    
    if (fullscrean)
    {
        flags = SDL_WINDOW_FULLSCREEN;
    }
    
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            printf("Renderer created!\n");
        }
        
        isRunning = true;
    }
    
    player = new Player(this);
    player->setWidth(PLAYER_WIDTH);
    player->setHeight(PLAYER_HEIGHT);
    player->setPos({TILESIZE * 2, TILESIZE * 2});
    
    camera = new Camera();
    camera->body = aabb{
        xy{0, 0},
        TILESIZE * WINDOW_SIZE_TILES_WIDTH,
        TILESIZE * WINDOW_SIZE_TILES_HEIGHT
    };
    camera->body.centerOn(player->getBody().center());
    
    map = 0;
    
//    {
//        int row = 0;
//        int col = 0;
//        xy start;
//        for (vector<int> i : maps[map]) {
//
//            for (int j : i) {
//                if (j == 'P') {
//                    start = {(float)row,(float)col};
//                    start *= (float)TILESIZE;
//                    printf("FOUND 'P' %i, %i\n", row, col);
//                }
//                ++col;
//            }
//            col = 0;
//            ++row;
//        }
//        player->setPos(start);
//    }
}

void Game::step()
{
    ++ticks;
//    printf("ticks: %li\n", ticks);
    
    // apply weak forces (input, dynamic) to player
    player->doWeakForces();
    
    // apply strong forces (corrective) to player
    player->applyForce(xy{0, keysDown[W] ? (float)FALL_VEL_SLOW : (float)FALL_VEL_FAST});
    
    // movement down
    if (player->getVel().y > 0) {
        
        // check for collision
        
        float dist = 0;
        while (getMapTileXY(player->getBody().bl()).y < maps[map].size()
               && getMapTileXY(player->getBody().br()).y < maps[map].size()
               && getMapTileVal({player->getBody().l(), player->getBody().b() + dist + 1}) != 1
               && getMapTileVal({player->getBody().r(), player->getBody().b() + dist + 1}) != 1) {
            ++dist;
        }
        dist = min(player->getVel().y, dist);
        if (dist < player->getVel().y) {
            // bounce (funny but dumb)
//            player->setVel({player->getVel().x, -(player->getVel().y/2)});
            player->setVel({player->getVel().x, 0});
            // may now undo jump lock as player is approaching ground
//            if (!keysDown[W])
//                player->jumped = false;
        }
        player->displace({0, dist});
    }
    // movement up
    else if (player->getVel().y < 0) {
        float dist = 0;
        while (getMapTileXY(player->getBody().tl()).y > 0
               && getMapTileXY(player->getBody().tr()).y > 0
               && getMapTileVal({player->getBody().l(), player->getBody().t() - dist - 1}) != 1
               && getMapTileVal({player->getBody().r(), player->getBody().t() - dist - 1}) != 1) {
            ++dist;
        }
        dist = min(-player->getVel().y, dist);
        if (-dist != player->getVel().y)
            player->setVel({player->getVel().x, 0});
        player->displace({0, -dist});
    }
    
    // movement right
    if (player->getVel().x > 0) {
        float dist = 0;
        while (getMapTileXY(player->getBody().tr()).x < maps[map][0].size()
               && getMapTileXY(player->getBody().br()).x < maps[map][0].size()
               && getMapTileVal({player->getBody().r() + dist + 1, player->getBody().t()}) != 1
               && getMapTileVal({player->getBody().r() + dist + 1, player->getBody().center().y}) != 1
               && getMapTileVal({player->getBody().r() + dist + 1, player->getBody().b()}) != 1) {
            ++dist;
        }
        dist = min(player->getVel().x, dist);
        if (dist != player->getVel().x)
            player->setVel({0, player->getVel().y});
        player->displace({dist, 0});
    }
    else if (player->getVel().x < 0) {
        float dist = 0;
        while (getMapTileXY(player->getBody().tl()).x > 0
               && getMapTileXY(player->getBody().bl()).x > 0
               && getMapTileVal({player->getBody().l() - dist - 1, player->getBody().t()}) != 1
               && getMapTileVal({player->getBody().l() - dist - 1, player->getBody().center().y}) != 1
               && getMapTileVal({player->getBody().l() - dist - 1, player->getBody().b()}) != 1) {
            ++dist;
        }
        dist = min(-player->getVel().x, dist);
        if (-dist != player->getVel().x)
            player->setVel({0, player->getVel().y});
        player->displace({-dist, 0});
    }
    
    // max velocity
    player->setVel({player->getVel().x < 0 ?
        max(player->getVel().x, (float)-MAX_VEL_X) : min(player->getVel().x, (float)MAX_VEL_X),
        player->getVel().y < 0 ? max(player->getVel().y, (float)-MAX_VEL_Y) : min(player->getVel().y, (float)MAX_VEL_Y)});
    
    // center camera on player
    camera->target = player->getBody().center();
    camera->update();
    
    // mouse indicator
    mouseIndicator.head = player->getBody().center();
    mouseIndicator.end = mouse;
}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    
    // draw map
    {
        int col = 0;
        int row = 0;
        int map = 0;
        for (vector<int> j : maps[map]) { // draw map tiles
            for (int k : j) {
                switch (k) {
                    case 0:
                        break;
                        
                    case 1: {
                        SDL_Rect rect = {
                            col * TILESIZE - (int)camera->body.l(),
                            row * TILESIZE - (int)camera->body.t(),
                            TILESIZE,
                            TILESIZE
                        };
                        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xa0);
                        SDL_RenderFillRect(renderer, &rect);
                    } break;
                        
                    case 2: {
                        Sint16 vx[] = {
                            (Sint16) (col * TILESIZE + TILESIZE / 2),
                            (Sint16) (col * TILESIZE + TILESIZE),
                            (Sint16) (col * TILESIZE),
                            (Sint16) (col * TILESIZE + TILESIZE / 2),
                        };
                        for (Sint16& i : vx)
                            i = getScreenXY(xy{(float)i, 0}).x;
                        Sint16 vy[] = {
                            (Sint16) (row * TILESIZE),
                            (Sint16) (row * TILESIZE + TILESIZE),
                            (Sint16) (row * TILESIZE + TILESIZE),
                            (Sint16) (row * TILESIZE)
                        };
                        for (Sint16& i : vy)
                            i = getScreenXY(xy{0, (float)i}).y;
                        filledPolygonRGBA(renderer, vx, vy, 4, 0xff, 0xff, 0xff, 0x60);
                        
                    } break;
                        
                    default:
                        break;
                }
                col++;
            }
            col = 0;
            row++;
        }
    }
    
    // draw player
    {
        Sint16 vx[] = {
            (Sint16)(player->getBody().l()),
            (Sint16)(player->getBody().r()),
            (Sint16)(player->getBody().r()),
            (Sint16)(player->getBody().l()),
            (Sint16)(player->getBody().l())
        };
        Sint16 vy[] = {
            (Sint16)(player->getBody().t()),
            (Sint16)(player->getBody().t()),
            (Sint16)(player->getBody().b()),
            (Sint16)(player->getBody().b()),
            (Sint16)(player->getBody().t())
        };
        
        for (Sint16& i : vx)
            i -= (int)camera->body.l();
        for (Sint16& i : vy)
            i -= (int)camera->body.t();
        
        
        filledPolygonRGBA(renderer, vx, vy, 5, 0xff, 0xff, 0xff, 0xfe);
//        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xa0);
    }
    
    
    
    // do dev mode
    if (DEV_MODE)
    {
        printf("FPS: %i\n", FPS);
        
        // test gfx performance
        {
            for (int i = 0; i < 300; ++i) {
                Sint16 first = (Sint16)(TILESIZE * WINDOW_SIZE_TILES_HEIGHT / ((rand() % 10)+1));
                Sint16 vx[] = {
                    first,
                    (Sint16)(first + rand() % 100),
                    (Sint16)(first + rand() % 100),
                    first
                };
                Sint16 vy[] = {
                    first,
                    (Sint16)(first + rand() % 100),
                    (Sint16)(first + rand() % 100),
                    first
                };
                filledPolygonRGBA(renderer, vx, vy, 4, 0xff, 0xff, 0xff, 0x20);
//                printf("%i ", rand() % 10);
            }
        }
        
        
        // draw camera give-box
        aabb cameraBox = aabb {
            0, 0, 114, 260
        };
        cameraBox.centerOn(xy{WINDOW_SIZE_TILES_WIDTH * TILESIZE / 2, WINDOW_SIZE_TILES_HEIGHT * TILESIZE / 2});
        rectangleRGBA(renderer, cameraBox.l(), cameraBox.t(), cameraBox.r(), cameraBox.b(), 0xff, 0xff, 0x00, 0xff);
        
        // draw player-overlapped map tiles
        xy tlxy = getMapTileXY(player->getBody().tl() - camera->body.tl());
        xy trxy = getMapTileXY(player->getBody().tr() - camera->body.tl());
        xy blxy = getMapTileXY(player->getBody().bl() - camera->body.tl());
        xy brxy = getMapTileXY(player->getBody().br() - camera->body.tl());
        
        SDL_Rect tlRect = SDL_Rect{
            (int)tlxy.x * TILESIZE,
            (int)tlxy.y * TILESIZE,
            TILESIZE,
            TILESIZE
        };
        SDL_Rect trRect = SDL_Rect{
            (int)trxy.x * TILESIZE,
            (int)trxy.y * TILESIZE,
            TILESIZE,
            TILESIZE
        };
        SDL_Rect blRect = SDL_Rect{
            (int)blxy.x * TILESIZE,
            (int)blxy.y * TILESIZE,
            TILESIZE,
            TILESIZE
        };
        SDL_Rect brRect = SDL_Rect{
            (int)brxy.x * TILESIZE,
            (int)brxy.y * TILESIZE,
            TILESIZE,
            TILESIZE
        };
        
        SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0xff, 0xff);
        
        SDL_RenderDrawRect(renderer, &tlRect);
        SDL_RenderDrawRect(renderer, &trRect);
        SDL_RenderDrawRect(renderer, &blRect);
        SDL_RenderDrawRect(renderer, &brRect);
        
        SDL_RenderDrawLine(renderer, 0, 0, mouse.x, mouse.y);
        
        // draw mouse indicator
        {
            lineRGBA(renderer,
                     mouseIndicator.head.x - camera->body.l(), mouseIndicator.head.y - camera->body.t(),
                     mouseIndicator.end.x, mouseIndicator.end.y,
                     0x00, 0xff, 0x00, 0xff);
            
            rectangleRGBA(renderer,
                          (int)mouseIndicator.end.x / TILESIZE * TILESIZE,
                          (int)mouseIndicator.end.y / TILESIZE * TILESIZE,
                          (int)mouseIndicator.end.x / TILESIZE * TILESIZE + TILESIZE,
                          (int)mouseIndicator.end.y / TILESIZE * TILESIZE + TILESIZE,
                          0xff, 0x00, 0xff, 0xff);
            
        }
    }
    else {
        FPS = DEFAULT_FPS;
    }
    
    SDL_RenderPresent(renderer);
}



void Game::handleEvents()
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            {
                isRunning = false;
                break;
            }
                
            case SDL_MOUSEBUTTONDOWN:
            {
                mouseDown = true;
                mouse = {(float)event.button.x, (float)event.button.y};
                break;
            }
                
            case SDL_MOUSEMOTION:
            {
                mouse = {(float)event.motion.x, (float)event.motion.y};
                break;
            }
                
            case SDL_MOUSEBUTTONUP:
            {
                mouseDown = false;
                break;
            }
                
            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                    {
                        isRunning = false;
                        break;
                    }
                    case SDLK_w:
                        keysDown[W] = true;
                        break;
                    case SDLK_s:
                        keysDown[S] = true;
                        break;
                    case SDLK_a:
                        keysDown[A] = true;
                        break;
                    case SDLK_d:
                        keysDown[D] = true;
                        break;
                    case SDLK_q:
                        keysDown[Q] = true;
                        if (DEV_MODE)
                            FPS = max(FPS - 1, 1);
                        break;
                    case SDLK_e:
                        keysDown[E] = true;
                        if (DEV_MODE)
                            FPS = min(FPS + 10, 120);
                        break;
                        
                    case SDLK_BACKQUOTE:
                        if (DEV_MODE)
                            DEV_MODE = false;
                        else
                            DEV_MODE = true;
                        
                    default: break;
                }
                break;
            }
                
            case SDL_KEYUP:
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_w:
                        keysDown[W] = false;
                        //                    keysDown[KeyboardController::keys::up] = false;
                        break;
                    case SDLK_s:
                        keysDown[S] = false;
                        //                    keysDown[KeyboardController::keys::down] = false;
                        break;
                    case SDLK_a:
                        keysDown[A] = false;
                        //                    keysDown[KeyboardController::keys::left] = false;
                        break;
                    case SDLK_d:
                        keysDown[D] = false;
                        //                    keysDown[KeyboardController::keys::right] = false;
                        break;
                    case SDLK_q:
                        keysDown[Q] = false;
                        break;
                    case SDLK_e:
                        keysDown[E] = false;
                        break;
                        
                    default: break;
                }
                break;
            }
                
            default:
                break;
        }
    }
}

xy Game::getMapTileXY(xy arg) {
    return {
        (float)((int)arg.x / (int) TILESIZE),
        (float)((int)arg.y / (int) TILESIZE)
    };
}

// TODO: update for camera z-position
xy Game::getScreenXY(xy arg) {
    return {arg.x - camera->body.l(), arg.y - camera->body.t()};
}

int Game::getScreenX(int arg) {
    return arg - camera->body.l();
}

int Game::getScreenY(int arg) {
    return arg - camera->body.t();
}

int Game::getMapTileVal(xy arg) {
    arg = getMapTileXY(arg);
    return maps[map][arg.y][arg.x];
}

bool Game::isGrounded(aabb arg) {
    return getMapTileVal({arg.l(), arg.b() + 1}) == 1 ||
           getMapTileVal({arg.r(), arg.b() + 1}) == 1;
}

int Game::groundedTile(xy arg) {
    return getMapTileVal(arg);
}
            
void Game::clean()
{
    delete player;
    delete camera;
    delete prevFrame;
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    printf("Game cleaned.\n");
}
