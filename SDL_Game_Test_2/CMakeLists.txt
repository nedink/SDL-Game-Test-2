cmake_minimum_required(VERSION 3.10)
project(SDL_Game_Test_2)

set(CMAKE_CXX_STANDARD 11)
set(SDL2 Library/Frameworks/SDL2.framework)
set(SDL2_IMAGE Library/Frameworks/SDL2_image.framework)

cmake_minimum_required(VERSION 2.8)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})

add_executable(SDL_Game_Test_2
        camera.cpp
        camera.h
        Game.cpp
        Game.h
        main.cpp
        maps.h
        player.cpp
        player.h
        util.h
        values.h
        bullet.cpp
        bullet.h
        util/*)

