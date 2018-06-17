cmake_minimum_required(VERSION 3.10)
project(SDL_Game_Test_2)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
#set(SDL2 Library/Frameworks/SDL2.framework)
#set(SDL2_IMAGE Library/Frameworks/SDL2_image.framework)
#set(SDL2GFX_DIRS Library/Frameworks/SDL2_gfx.framework)
#set(SDL2GFX_LIBS Library/Frameworks/SDL2_gfx.framework/Headers)

add_executable(SDL_Game_Test_2
        util/physics.h
        util/util.h
        bullet.cpp
        bullet.h
        camera.cpp
        camera.h
        Game.cpp
        Game.h
        main.cpp
        maps.h
        player.cpp
        player.h
        values.h
        )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SDL_Game_Test_2_SOURCE_DIR}/cmake")

#INCLUDE(FindPkgConfig)

#PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
#PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_gfx REQUIRED)

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_GFX_INCLUDE_DIR} )

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${SDL2_GFX_LIBRARIES})

#INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})