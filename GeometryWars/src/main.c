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
    game->assets    = assetmanager_init();

    assetmanager_add_shader(&game->assets, "Default", "./res/common.vs", "./res/common.fs");
    assetmanager_add_freetypefont(&game->assets, "Font", "./res/Roboto-Bold.ttf", 15);

    game->font = assetmanager_get_freetypefont(&game->assets, "Font");

    // Setting up player
    game_system_spawn_player(game, application_get_window(app));
}

void app_update(application_t *app)
{
    assert(app->game);

    game_t *game    = (game_t *)app->game;
    f32 dt          = application_get_dt(app);
    f32 fps         = application_get_fps(app);
    window_t *win   = application_get_window(app);

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "FPS: %2.0f",fps);

    glfreetypefont_set_dynamic_text(game->font, buffer, (vec2f_t ){0.8f, 0.9f}, COLOR_WHITE);

    // update window update input buffer
    window_update_user_input(win);

    entitymanager_update(&game->manager);

    // spawns 
    game_system_enemy_spawner(game, dt);

    // enemy update
    game_system_enemy_update(game, dt);

    // player update
    game_system_player_update(game, dt);

    // bullet update
    game_system_bullet_update(game, dt);

    // collision
    game_system_collision(game, dt);

    // explosion
    game_system_explosion_update(game);

    // ult
    game_system_ult_update(game);
}

void app_render(application_t *app)
{
    assert(app->game);

    game_t *game                = (game_t *)app->game;
    entitymanager_t *manager    = &game->manager;
    s_renderer2d_t *renderer    = &game->renderer;

    s_renderer2d_draw(renderer, manager);
    glfreetypefont_draw(game->font);
}

void app_shutdown(application_t *app)
{
    assert(app->game);
    game_t *game = (game_t *)app->game;

    entitymanager_destroy(&game->manager);
    s_renderer2d_destroy(&game->renderer);

    assetmanager_destroy(&game->assets);

    game->font = NULL;
}

int main(void)
{
    game_t GeometryWar;

    // Setup application
    application_t app = {
        .window_title = "GeometryWar",
        .window_width = 700,
        .window_height = 800,

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
