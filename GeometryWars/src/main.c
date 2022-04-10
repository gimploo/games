#include "../lib/application.h"
#include "../lib/ecs/entitymanager.h"
#include "../lib/ecs/systems.h"

#include "game.h"


void app_init(application_t *app)
{
    assert(app->game);

    application_set_background(app, COLOR_BLACK);

    game_t *game    = (game_t *)app->game;
    game->manager   = entitymanager_init(COUNT);
    game->renderer  = s_renderer2d_init();
    game->font      = glfreetypefont_init("./res/Roboto-Bold.ttf", 20);

    // Setting up player
    game_system_spawn_player(game, app->__window_handle);
}

void app_update(application_t *app)
{
    assert(app->game);

    game_t *game    = (game_t *)app->game;
    f32 dt          = application_get_dt(app);
    f32 fps         = application_get_fps(app);

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "FPS: %2.0f",fps);

    glfreetypefont_set_text(&game->font, buffer, (vec2f_t ){0.8f, 0.8f}, COLOR_WHITE);

    entitymanager_update(&game->manager);

    /*// spawns */
    game_system_enemy_spawner(game, dt);

    /*// enemy update*/
    game_system_enemy_update(game, dt);

    /*// player update*/
    game_system_player_update(game, dt);

    /*// bullet update*/
    game_system_bullet_update(game, dt);

    /*// collision*/
    game_system_collision(game, dt);

    /*// explosion*/
    game_system_explosion_update(game);

    /*// ult*/
    game_system_ult_update(game);
}

void app_render(application_t *app)
{
    assert(app->game);

    game_t *game                = (game_t *)app->game;
    entitymanager_t *manager    = &game->manager;
    s_renderer2d_t *renderer    = &game->renderer;

    s_renderer2d_draw(renderer, manager);
    glfreetypefont_draw(&game->font);
}

void app_shutdown(application_t *app)
{
    assert(app->game);
    game_t *game = (game_t *)app->game;

    entitymanager_destroy(&game->manager);
    s_renderer2d_destroy(&game->renderer);
    glfreetypefont_destroy(&game->font);
}

int main(void)
{
    game_t GeometryWar;

    // Setup application
    application_t app = {
        .title = "GeometryWar",
        .width = 700,
        .height = 800,

        .game = &GeometryWar,

        .init = app_init,
        .update = app_update,
        .render = app_render,
        .shutdown = app_shutdown
    };

    // Run the application
    application_run(&app);


    return 0;
}
