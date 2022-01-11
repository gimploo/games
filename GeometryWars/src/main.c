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
    entity_t        *player     = entitymanager_add_entity(&game->manager, PLAYER);
   c_transform_t   *transform  = c_transform_init(vec3f(0.0f), 0.6f, PI / 4, 0.02f, PI / 4);
    /*c_transform_t   *transform  = c_transform_init(vec3f(0.0f), 0.6f, PI / 4, 0.0f, 0.0f);*/
    c_shape2d_t     *shape      = c_shape2d_init(transform->position, SQUARE, 0.2, ((vec4f_t ){1.0f, 0.2f, 0.0f, 1.0f}));
    c_shader_t      *shader     = c_shader_init("./res/player.vs", "./res/player.fs");
    c_input_t       *input      = c_input_init(app->__window_handle);
    c_boxcollider2d_t *collider = c_boxcollider2d_init(transform->position, shape->radius);

    entity_add_component(player, transform,     c_transform_t );
    entity_add_component(player, shape,         c_shape2d_t );
    entity_add_component(player, shader,        c_shader_t );
    entity_add_component(player, input,         c_input_t );
    entity_add_component(player, collider,      c_boxcollider2d_t );

    game->player = player;
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

