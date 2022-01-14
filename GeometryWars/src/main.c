#include "../lib/application.h"
#include "../lib/ecs/entitymanager.h"
#include "../lib/ecs/systems.h"


#include "game.h"

//TODO: component managent system
//TODO: properly destroy shaders when done
//TODO: rotation 
//TODO: lower the color opacity of the bullet

void app_init(application_t *app)
{
    assert(app->game);
    game_t *game = (game_t *)app->game;
    game->manager = entitymanager_init(COUNT);
    game->renderer = s_renderer2d_init();

    // Setting up player
    game_system_spawn_player(game, app->__window_handle);
}

void app_update(application_t *app)
{
    assert(app->game);
    game_t *game = (game_t *)app->game;

    f32 dt = application_get_dt(app);

    entitymanager_update(&game->manager);

    // player update
    game_system_player_update(game, dt);

    // bullet update
    game_system_bullet_update(game, dt);

    // spawns 
    game_system_enemy_spawner(game);

    // collision
    game_system_collision(game);

}

void app_render(application_t *app)
{
    assert(app->game);
    game_t *game = (game_t *)app->game;
    entitymanager_t *manager = &game->manager;
    s_renderer2d_t  *renderer = &game->renderer;

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

    window_t win        = window_init("Geometry Wars", 700, 800, SDL_INIT_VIDEO);
    window_set_background(&win, vec4f(0.0f));
    application_t app   = application_init(&win);
    application_pass_game(&app, &GeometryWar);

    application_run(&app);


    return 0;
}

