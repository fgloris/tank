#pragma once
#include "config.hpp"

enum MenuOptions{
    MENU_HOME,MENU_README,MENU_DOUBLE,MENU_SINGLE,MENU_ONLINE,MENU_EXIT,
};
struct Menu{
    SDL_Texture * texture;
    MenuOptions options = MENU_HOME;
    SDL_Color color = {};

};
enum PauseOptions{
    PAUSE_HOME,RESUME,OPTIONS,
};
struct PauseMenu{

};