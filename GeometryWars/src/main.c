#include "../lib/application.h"
#include "../lib/ecs/entitymanager.h"
#include "../lib/ecs/systems.h"

typedef enum game_entity_type {

    GET_PLAYER,
    GET_ENEMY,
    GET_ENEMY_EXPLODED,
    GET_BULLET,
    GET_ULT,
    GET_COUNT

} game_entity_type;

// GAME
typedef struct game_t {

    entity_t        *player;
    entitymanager_t manager;


} game_t;

void print_entity(void *arg)
{
    entity_t *e = (entity_t *)arg;

    printf("%li\n", *e->id);
    printf("%li\n", e->tag);
    printf("%li\n", e->components.len);

}

void init(application_t *app)
{
    game_t *game = (game_t *)app->game;
    game->manager = entitymanager_init(GET_COUNT);

    // Setting up player
    entity_t *player            = entitymanager_add_entity(&game->manager, GET_PLAYER);
    c_transform_t *transform    = c_transform_init(vec3f(0.0f), vec3f(0.0f), 0.0f);
    c_shape2d_t   *shape        = c_shape2d_init(transform, CIRCLE, 0.2, vec4f(0.0f));
    c_shader_t    *shader       = c_shader_init("../res/player.vs", "../res/player.fs");
    entity_add_component(player, transform, c_transform_t );
    entity_add_component(player, shape, c_shape2d_t );
    entity_add_component(player, shader, c_shader_t );
    game->player = player;
}

void update(application_t *app)
{
    game_t *game = (game_t *)app->game;

    entitymanager_update(&game->manager);
}

void render(application_t *app)
{
    game_t *game = (game_t *)app->game;

    s_renderer2d_draw(&game->manager);
}

void shutdown(application_t *app)
{
    game_t *game = app->game;

    entitymanager_destroy(&game->manager);
}

int main(void)
{
    game_t GeometryWar;

    window_t win = window_init("Geometry Wars", 700, 800, SDL_INIT_VIDEO);
    application_t app = application_init(&win);

    application_pass_game(&app, &GeometryWar);

    application_run(&app);

    return 0;
}

