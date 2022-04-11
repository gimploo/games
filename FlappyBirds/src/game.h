#pragma once
#include "../lib/poggen.h"

// scenes
#include "./scenes/menu.h"
#include "./scenes/gameover.h"
#include "./scenes/playing.h"
#include "./scenes/playerready.h"


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
