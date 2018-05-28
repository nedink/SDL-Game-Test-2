cmake_minimum_required(VERSION 3.10)
project(SDL_Game_Test_2)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

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
        values.h)
