#define WINDOW_SDL
#include <poglib/application.h>
#include <poglib/poggen.h>
#include "./scenes/gameplay.h"

typedef struct game_t {

    poggen_t *engine;

} game_t ;

void FlappyBird_init(application_t *app) 
{
    game_t game = {
        .engine = poggen_init()
    };

    //scenes
    poggen_add_scene(game.engine, gameplay);

    //common assets
    assetmanager_add_texture2d(
            &game.engine->assets, "background", "res/background.png");
    assetmanager_add_texture2d(
            &game.engine->assets, "ground", "res/ground.png");
    assetmanager_add_texture2d(
            &game.engine->assets, "bird", "res/bird.png");
    assetmanager_add_shader(
            &game.engine->assets, "shader", "res/common.vs", "res/common.fs");

    application_pass_content(app, &game);
}

void FlappyBird_update(application_t *app) 
{
    window_t *win = application_get_window(app);
    game_t *c = application_get_content(app);
    const f32 dt = application_get_dt(app);

    poggen_update(c->engine, dt);
}

void FlappyBird_render(application_t *app) 
{
    window_t *win = application_get_window(app);
    game_t *c = application_get_content(app);

    poggen_render(c->engine);
}

void FlappyBird_destroy(application_t *app) 
{
    window_t *win = application_get_window(app);
    game_t *c = application_get_content(app);

    poggen_destroy(c->engine);
}

int main(void)
{
    application_t app = {
        .window = {
            .title = "FlappyBird",
            .width = 512,
            .height = 720,
            .aspect_ratio = (f32)512 / (f32)720,
            .fps_limit = 60 
        },   
        .content = {
            .size = sizeof(game_t )
        },
        .init       = FlappyBird_init,
        .update     = FlappyBird_update,
        .render     = FlappyBird_render,
        .destroy    = FlappyBird_destroy
    };

    application_run(&app);

    return 0;
}

