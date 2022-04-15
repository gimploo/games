#include "../lib/application.h"
#include "./game.h"

#define WINDOW_WIDTH    450
#define WINDOW_HEIGHT   700

void app_init(application_t *app)
{
    game_t *game = app->game;
    game->engine = poggen_init();

    assetmanager_t *assetmanager = &game->engine->assets;
    assetmanager_add_texture2d(assetmanager, "Spritesheet", "./res/spritesheet.png"); 

    poggen_t *engine = game->engine;

    poggen_add_scene(engine, menu);
    /*poggen_add_scene(engine, playerready);*/
    /*poggen_add_scene(engine, playing);*/
    /*poggen_add_scene(engine, gameover);*/

}

void app_update(application_t *app)
{
    game_t *game = app->game;
    poggen_t *engine = game->engine;

    poggen_update_currentscene(engine);

}

void app_render(application_t *app)
{
    game_t *game = app->game;
    poggen_t *engine = game->engine;

    poggen_render_currentscene(engine);
    
}

void app_shutdown(application_t *app)
{
    game_t *game = app->game;

    poggen_destroy(game->engine);
}

int main(void)
{
    game_t FlappyBird;

    application_t app = {

        .window_title      = "Flappy bird",
        .window_width      = WINDOW_WIDTH,
        .window_height     = WINDOW_HEIGHT,
        
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
