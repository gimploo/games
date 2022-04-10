#include "../lib/application.h"
#include "./game.h"

#define WINDOW_WIDTH    450
#define WINDOW_HEIGHT   700


void app_init(application_t *app)
{
    game_t *game = app->game;
    game->engine = poggen_init();

    assetmanager_t *assetmanager = &game->engine.assets;
    assetmanager_add_texture2d(assetmanager, "Spritesheet", "./res/spritesheet.png"); 

    poggen_t *engine = &game->engine;
    poggen_add_scene(engine, MENU);
    poggen_add_scene(engine, PLAYERREADY);
    poggen_add_scene(engine, PLAYING);
    poggen_add_scene(engine, GAMEOVER);

    engine->current_scene = (scene_t *)map_get_value(&engine->scenes, "MENU");

}

void app_update(application_t *app)
{
    game_t *game = app->game;
    poggen_t *engine = &game->engine;

    scene_t *current_scene = engine->current_scene;
    switch(scene_get_type(current_scene))
    {
        case MENU:

        break;

        case PLAYERREADY:

        break;

        case PLAYING:

        break;

        case GAMEOVER:
            
        break;

        default: eprint("scene `%s` type not accounted for (%i)", current_scene->label, scene_get_type(current_scene));
    }

}

void app_render(application_t *app)
{
    game_t *game = app->game;
}

void app_shutdown(application_t *app)
{
    game_t *game = app->game;

    poggen_destroy(&game->engine);
}

int main(void)
{
    game_t FlappyBird;

    application_t app = {

        .title      = "Flappy bird",
        .width      = WINDOW_WIDTH,
        .height     = WINDOW_HEIGHT,
        
        // passing the game
        .game       = &FlappyBird,

        .init       = app_init,
        .update     = app_update,
        .render     = app_render,
        .shutdown   = app_shutdown
    };

    application_run(&app);

    return 0;
}
