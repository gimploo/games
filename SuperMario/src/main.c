#include <poglib/application.h>
#include <poglib/poggen.h>

#include "./scenes/menu.h"

typedef struct SuperMario {
    poggen_t *engine;
} SuperMario;

void app_init(application_t *app)
{
    SuperMario *game = app->game;
    game->engine = poggen_init();

    poggen_add_scene(game->engine, menu);

}

void app_update(application_t *app)
{
    SuperMario *game = app->game;

    poggen_update(game->engine);
}

void app_render(application_t *app)
{
    SuperMario *game = app->game;

    poggen_render(game->engine);
}

void app_destroy(application_t *app)
{
    SuperMario *game = app->game;
    poggen_destroy(game->engine);
}

int main(void)
{
    application_t app = {0};

    SuperMario game = {0};

    app = (application_t ){

        .window_width   = 800,
        .window_height  = 700,
        .window_title   = "SuperMario",

        .game           = &game,

        .init       = app_init,
        .update     = app_update,
        .render     = app_render,
        .destroy    = app_destroy
    };

    application_run(&app);

    return 0;
}
