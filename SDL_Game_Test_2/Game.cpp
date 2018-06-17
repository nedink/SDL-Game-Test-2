#include <stdio.h>
#include <math.h>

#include <string>
#include <SDL2_gfx/SDL2_gfxPrimitives.h>

#include "Game.h"
#include "maps.h"
#include "player.h"
#include "bullet.h"
#include "camera.h"

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;

Game::Game()
{}

//Game::Game(const Game &game)
//{
//    this->player = new Player(*player);
//    this->camera = new Camera(*camera);
//}

Game::~Game()
{}

void Game::init()
{
    int flags = 0;
//    flags += SDL_WINDOW_FULLSCREEN;
    
    window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE_TILES_WIDTH*TILESIZE, WINDOW_SIZE_TILES_HEIGHT*TILESIZE, flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        printf("Renderer created!\n");
    }
    
    isRunning = true;
    
    player = new Player(this);
    player->setWidth(PLAYER_WIDTH);
    player->setHeight(PLAYER_HEIGHT);
    player->setPos({TILESIZE * 2, TILESIZE * 2});
    
    camera = new Camera();
    camera->setPos({0, 0});
    camera->setWidth(TILESIZE * WINDOW_SIZE_TILES_WIDTH);
    camera->setHeight(TILESIZE * WINDOW_SIZE_TILES_HEIGHT);
    camera->centerOn(player->center());
    
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
    reloadTicks -= reloadTicks > 0 ? 1 : 0;
    
    
    // update bullets
    for (auto bullet : bullets) {
        bullet->update();
//        printf("%f, %f\n", bullet)
    }
    
    // spawn bullet
    if (mouseDown && reloadTicks == 0) {
        reloadTicks = 10;
        Bullet* b = new Bullet();
        b->setWidth(4);
        b->setHeight(4);
        b->centerOn(player->center());
        printf("player pos: %f, %f\n", player->getPos().x, player->getPos().y);
        b->setVel(geom::xy::ofAngle(geom::xy::angleOf(mouse - getScreenXy(b->getPos()))) + (rand() % 50) * .001 - 0.05 );
//        geom::xy thing = ;
//        printf("%f, %f\n", thing.x, thing.y);
		
        float bulletVel = 16.0f;
        b->setVel(b->getVel() * bulletVel);
//        printf("bullet vel: %f, %f\n", b->getVel().x, b->getVel().y);
        geom::xy mouseXY = getScreenXy(mouse);
        geom::xy spawnPos = getScreenXy(player->getPos());
        bullets.push_back(b);
    }
    
    // apply weak forces (input, dynamic) to player
    player->doWeakForces();
    
    // apply strong forces (corrective) to player
    player->applyForce(geom::xy{0, keysDown[W] ? (float)FALL_VEL_SLOW : (float)FALL_VEL_FAST});
    
    // movement down
    if (player->getVel().y > 0) {
        
        // check for collision
        
        float dist = 0;
        while (getMapTileXy(player->bl()).y < maps[map].size()
               && getMapTileXy(player->br()).y < maps[map].size()
               && getMapTileVal({player->l(), player->b() + dist + 1}) != 1
               && getMapTileVal({player->r(), player->b() + dist + 1}) != 1) {
            ++dist;
        }
        dist = std::min(player->getVel().y, dist);
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
        while (getMapTileXy(player->tl()).y > 0
               && getMapTileXy(player->tr()).y > 0
               && getMapTileVal({player->l(), player->t() - dist - 1}) != 1
               && getMapTileVal({player->r(), player->t() - dist - 1}) != 1) {
            ++dist;
        }
        dist = std::min(-player->getVel().y, dist);
        if (-dist != player->getVel().y)
            player->setVel({player->getVel().x, 0});
        player->displace({0, -dist});
    }
    
    // movement right
    if (player->getVel().x > 0) {
        float dist = 0;
        while (getMapTileXy(player->tr()).x < maps[map][0].size()
               && getMapTileXy(player->br()).x < maps[map][0].size()
               && getMapTileVal({player->r() + dist + 1, player->t()}) != 1
               && getMapTileVal({player->r() + dist + 1, player->center().y}) != 1
               && getMapTileVal({player->r() + dist + 1, player->b()}) != 1) {
            ++dist;
        }
        dist = std::min(player->getVel().x, dist);
        if (dist != player->getVel().x)
            player->setVel({0, player->getVel().y});
        player->displace({dist, 0});
    }
    else if (player->getVel().x < 0) {
        float dist = 0;
        while (getMapTileXy(player->tl()).x > 0
               && getMapTileXy(player->bl()).x > 0
               && getMapTileVal({player->l() - dist - 1, player->t()}) != 1
               && getMapTileVal({player->l() - dist - 1, player->center().y}) != 1
               && getMapTileVal({player->l() - dist - 1, player->b()}) != 1) {
            ++dist;
        }
        dist = std::min(-player->getVel().x, dist);
        if (-dist != player->getVel().x)
            player->setVel({0, player->getVel().y});
        player->displace({-dist, 0});
    }
    
    // max velocity
    player->setVel({player->getVel().x < 0 ?
        std::max(player->getVel().x, (float)-MAX_VEL_X) : std::min(player->getVel().x, (float)MAX_VEL_X),
        player->getVel().y < 0 ? std::max(player->getVel().y, (float)-MAX_VEL_Y) : std::min(player->getVel().y, (float)MAX_VEL_Y)});
    
    
    
    // center camera on player
    camera->target = player->center();
    camera->update();
    
    // mouse indicator
    mouseIndicator.start = player->center();
    mouseIndicator.end = mouse;
}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    
    // draw bullets
    for (auto& bullet : bullets) {
        geom::aabb rectAabb = bullet->getBody();
        rectAabb = getScreenXy(rectAabb.tl());
//        rectAabb -= camera->tl();
        
        SDL_Rect rect = rectAabb.toSDL();
        
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderDrawLine(renderer,
                           (int)getScreenXy(bullet->getPos()).x,
                           (int)getScreenXy(bullet->getPos()).y,
                           (int)getScreenXy((bullet->getPos() - bullet->getVel())).x,
                           (int)getScreenXy((bullet->getPos() - bullet->getVel())).y);
    }
    
    // draw map
    {
        int col = 0;
        int row = 0;
        int map = 0;
        for (std::vector<int> j : maps[map]) { // draw map tiles
            for (int k : j) {
                switch (k) {
                    case 0:
                        break;
                        
                    case 1: {
                        SDL_Rect rect = {
                            col * TILESIZE - (int)camera->l(),
                            row * TILESIZE - (int)camera->t(),
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
                            i = getScreenXy(geom::xy{(float)i, 0}).x;
                        Sint16 vy[] = {
                            (Sint16) (row * TILESIZE),
                            (Sint16) (row * TILESIZE + TILESIZE),
                            (Sint16) (row * TILESIZE + TILESIZE),
                            (Sint16) (row * TILESIZE)
                        };
                        for (Sint16& i : vy)
                            i = getScreenXy(geom::xy{0, (float)i}).y;
                        SDL_Rect rect = {col * TILESIZE, row * TILESIZE, TILESIZE, TILESIZE};
                        SDL_RenderDrawRect(renderer, &rect);
//                        filledPolygonRGBA(renderer, vx, vy, 4, 0xff, 0xff, 0xff, 0x60);
                        
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
            (Sint16)(player->l()),
            (Sint16)(player->r()),
            (Sint16)(player->r()),
            (Sint16)(player->l()),
            (Sint16)(player->l())
        };
        Sint16 vy[] = {
            (Sint16)(player->t()),
            (Sint16)(player->t()),
            (Sint16)(player->b()),
            (Sint16)(player->b()),
            (Sint16)(player->t())
        };
        
        for (Sint16& i : vx)
            i -= (int)camera->l();
        for (Sint16& i : vy)
            i -= (int)camera->t();

        SDL_Rect rect = player->getBody().toSDL();

//        SDL_RenderDrawRect(renderer, &rect);
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
//                filledPolygonRGBA(renderer, vx, vy, 4, 0xff, 0xff, 0xff, 0x20);
//                printf("%i ", rand() % 10);
            }
        }
        
        
        // draw camera give-box
        geom::aabb cameraBox = {
            0, 0, 114, 260
        };
        cameraBox.centerOn(geom::xy{WINDOW_SIZE_TILES_WIDTH * TILESIZE / 2, WINDOW_SIZE_TILES_HEIGHT * TILESIZE / 2});
        SDL_Rect rect = cameraBox.toSDL();
        SDL_RenderDrawRect(renderer, &rect);
//        rectangleRGBA(renderer, cameraBox.l(), cameraBox.t(), cameraBox.r(), cameraBox.b(), 0xff, 0xff, 0x00, 0xff);
        
        // draw player-overlapped map tiles
        geom::xy tlxy = getMapTileXy(player->tl() - camera->tl());
        geom::xy trxy = getMapTileXy(player->tr() - camera->tl());
        geom::xy blxy = getMapTileXy(player->bl() - camera->tl());
        geom::xy brxy = getMapTileXy(player->br() - camera->tl());
        
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
            geom::xy start = getScreenXy(mouseIndicator.start);
            SDL_RenderDrawLine(renderer, start.x, start.y, mouse.x, mouse.y);
//            lineRGBA(renderer,
//                     start.x, start.y,
//                     mouse.x, mouse.y,
//                     0x00, 0xff, 0x00, 0xff);

//            rectangleRGBA(renderer,
//                          (int)mouseIndicator.end.x / TILESIZE * TILESIZE,
//                          (int)mouseIndicator.end.y / TILESIZE * TILESIZE,
//                          (int)mouseIndicator.end.x / TILESIZE * TILESIZE + TILESIZE,
//                          (int)mouseIndicator.end.y / TILESIZE * TILESIZE + TILESIZE,
//                          0xff, 0x00, 0xff, 0xff);
            
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
                            FPS = std::max(FPS - 1, 1);
                        break;
                    case SDLK_e:
                        keysDown[E] = true;
                        if (DEV_MODE)
                            FPS = std::min(FPS + 10, 120);
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
                        break;
                    case SDLK_s:
                        keysDown[S] = false;
                        break;
                    case SDLK_a:
                        keysDown[A] = false;
                        break;
                    case SDLK_d:
                        keysDown[D] = false;
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

geom::xy Game::getMapTileXy(geom::xy arg) {
    return {
        (float)((int)arg.x / (int) TILESIZE),
        (float)((int)arg.y / (int) TILESIZE)
    };
}

// TODO: update for camera z-position
geom::xy Game::getScreenXy(geom::xy gameXy) {
    return gameXy - camera->tl();
}

geom::xy Game::getGameXy(geom::xy screenXy) {
    return camera->tl() - screenXy;
}

int Game::getScreenX(int arg) {
    return arg - camera->l();
}

int Game::getScreenY(int arg) {
    return arg - camera->t();
}

int Game::getMapTileVal(geom::xy arg) {
    arg = getMapTileXy(arg);
    return maps[map][arg.y][arg.x];
}

bool Game::isGrounded(geom::aabb arg) {
    return getMapTileVal({arg.l(), arg.b() + 1}) == 1 ||
           getMapTileVal({arg.r(), arg.b() + 1}) == 1;
}

int Game::groundedTile(geom::xy arg) {
    return getMapTileVal(arg);
}
            
void Game::clean()
{
    delete player;
    delete camera;
    delete prevFrame;
    for (Bullet* b : bullets)
        delete b;
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    printf("Game cleaned.\n");
}
