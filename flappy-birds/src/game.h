#pragma once
#include "../lib/poggen.h"


typedef struct game_t {

    poggen_t engine;

} game_t ;

typedef enum scene_types {

        MENU = 0,
        PLAYERREADY,
        PLAYING,
        GAMEOVER,
        TOTAL

} scene_types;
