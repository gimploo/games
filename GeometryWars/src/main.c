#include "../lib/application.h"
#include "../lib/ecs/entitymanager.h"
#include "../lib/ecs/systems.h"

#include "game.h"


void app_init(application_t *app)
{
    assert(app->game);

    game_t *game    = (game_t *)app->game;
    game->manager   = entitymanager_init(COUNT);
    game->renderer  = s_renderer2d_init();

    // Setting up player
    game_system_spawn_player(game, app->__window_handle);
}

void app_update(application_t *app)
{
    assert(app->game);

    game_t *game    = (game_t *)app->game;
    f32 dt          = application_get_dt(app);

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
    game_system_collision(game);

    // explosion
    game_system_explosion_update(game);

}

void app_render(application_t *app)
{
    assert(app->game);

    game_t *game                = (game_t *)app->game;
    entitymanager_t *manager    = &game->manager;
    s_renderer2d_t *renderer    = &game->renderer;

    s_renderer2d_draw(renderer, manager);
}

void app_shutdown(application_t *app)
{
    assert(app->game);
    game_t *game = (game_t *)app->game;

    entitymanager_destroy(&game->manager);
}

int main(void)
{
    game_t GeometryWar;

    // Setup window
    window_t win = window_init("Geometry Wars", 700, 800, SDL_INIT_VIDEO);
    window_set_background(&win, COLOR_BLACK);

    // Setup application
    application_t app = application_init(&win);

    // Pass the game to the application
    application_pass_game(&app, &GeometryWar);

    // Run the application
    application_run(&app);


    //Cleanup
    window_destroy(&win);

    return 0;
}

