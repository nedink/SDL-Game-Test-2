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
        renderer = SDL_CreateRenderer(window, -1, 0);
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
        float dist = 0;
        while (mapTileXY(player->getBody().bl()).y < maps[map].size()
               && mapTileXY(player->getBody().br()).y < maps[map].size()
               && mapTile({player->getBody().l(), player->getBody().b() + dist + 1}) != 1
               && mapTile({player->getBody().r(), player->getBody().b() + dist + 1}) != 1) {
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
        while (mapTileXY(player->getBody().tl()).y > 0
               && mapTileXY(player->getBody().tr()).y > 0
               && mapTile({player->getBody().l(), player->getBody().t() - dist - 1}) != 1
               && mapTile({player->getBody().r(), player->getBody().t() - dist - 1}) != 1) {
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
        while (mapTileXY(player->getBody().tr()).x < maps[map][0].size()
               && mapTileXY(player->getBody().br()).x < maps[map][0].size()
               && mapTile({player->getBody().r() + dist + 1, player->getBody().t()}) != 1
               && mapTile({player->getBody().r() + dist + 1, player->getBody().center().y}) != 1
               && mapTile({player->getBody().r() + dist + 1, player->getBody().b()}) != 1) {
            ++dist;
        }
        dist = min(player->getVel().x, dist);
        if (dist != player->getVel().x)
            player->setVel({0, player->getVel().y});
        player->displace({dist, 0});
    }
    else if (player->getVel().x < 0) {
        float dist = 0;
        while (mapTileXY(player->getBody().tl()).x > 0
               && mapTileXY(player->getBody().bl()).x > 0
               && mapTile({player->getBody().l() - dist - 1, player->getBody().t()}) != 1
               && mapTile({player->getBody().l() - dist - 1, player->getBody().center().y}) != 1
               && mapTile({player->getBody().l() - dist - 1, player->getBody().b()}) != 1) {
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
    
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    
    // draw map
    {
        int col = 0;
        int row = 0;
        int map = 0;
        for (vector<vector<int>> i : maps) // draw map tiles
        {
            for (vector<int> j : i) {
                for (int k : j) {
                    switch (k) {
                            
                        case 0:
                            break;
                            
                        case 1: {
                            SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
                            SDL_Rect rect = {
                                col * TILESIZE - (int)camera->body.l(),
                                row * TILESIZE - (int)camera->body.t(),
                                TILESIZE,
                                TILESIZE
                            };
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
                                i -= (int)camera->body.l();
                            Sint16 vy[] = {
                                (Sint16) (row * TILESIZE),
                                (Sint16) (row * TILESIZE + TILESIZE),
                                (Sint16) (row * TILESIZE + TILESIZE),
                                (Sint16) (row * TILESIZE)
                            };
                            for (Sint16& i : vy)
                                i -= (int)camera->body.t();
                            filledPolygonRGBA(renderer, vx, vy, 4, 0xff, 0x00, 0x00, 0xff);
                            
                        } break;
                            
                        default:
                            break;
                    }
                    if (k == 1) {
                        
                    }
                    col++;
                }
                col = 0;
                row++;
            }
            row = 0;
            map++;
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
        
        if (grounded(player->getBody()))
        	filledPolygonRGBA(renderer, vx, vy, 5, 0x00, 0xff, 0x00, 0xc0);
        else
            filledPolygonRGBA(renderer, vx, vy, 5, 0x00, 0xff, 0x00, 0x80);
    }
    
    
    // do dev mode
    if (DEV_MODE)
    {
        // draw player overlapping map tiles
        xy tlxy = mapTileXY(player->getBody().tl() - camera->body.tl());
        xy trxy = mapTileXY(player->getBody().tr() - camera->body.tr());
        xy blxy = mapTileXY(player->getBody().bl() - camera->body.bl());
        xy brxy = mapTileXY(player->getBody().br() - camera->body.br());
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
                        break;
                    case SDLK_e:
                        keysDown[E] = true;
                        break;
                    default: break;
                }
                break;
            }
                
            case SDL_KEYUP:
            {
//                printf("key up\n");
                
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

xy Game::mapTileXY(xy xy)
{
    return {
        (float)((int)xy.x / (int) TILESIZE),
        (float)((int)xy.y / (int) TILESIZE)
    };
}

int Game::mapTile(xy xy)
{
    xy = mapTileXY(xy);
    return maps[map][xy.y][xy.x];
}

bool Game::grounded(aabb aabb) {
    return mapTile({aabb.l(), aabb.b() + 1}) == 1 || mapTile({aabb.r(), aabb.b() + 1}) == 1;
}

int Game::groundedTile(xy xy) {
    return mapTile(xy);
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
