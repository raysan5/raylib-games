cmake_minimum_required(VERSION 3.25)
project(RayligGames)

find_package(raylib REQUIRED)

option(PLATFORM_DESKTOP "Build for Desktop" ON)
option(PLATFORM_WEB "Build for Desktop" OFF)

add_subdirectory(cat_vs_roomba)
add_subdirectory(classics)
add_subdirectory(drturtle)
add_subdirectory(just_do)
add_subdirectory(koala_seasons)
add_subdirectory(light_my_ritual)
add_subdirectory(repair)
add_subdirectory(retro_maze_3d)
add_subdirectory(skully_escape)
add_subdirectory(transmission)
add_subdirectory(wave_collector)
