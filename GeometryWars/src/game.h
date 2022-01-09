#pragma once
#include "../lib/ecs/entitymanager.h"
#include "../lib/ecs/systems.h"

#define MAX_BULLET_SPEED vec3f(0.0f)
#define BULLET_SIDE 0.02f

typedef enum game_entity_type {

    PLAYER,
    ENEMY,
    ET_ENEMY_EXPLODED,
    BULLET,
    ULT,

    COUNT

} game_entity_type;

typedef struct game_t {

    entity_t        *player;
    entitymanager_t manager;

    s_renderer2d_t renderer;

} game_t;



void game_system_collision(entitymanager_t *manager, entity_t *player)
{
    assert(manager);

    c_boxcollider2d_t *pbox = (c_boxcollider2d_t *)entity_get_component(player, c_boxcollider2d_t );
    assert(pbox);

    list_t *bullets = entitymanager_get_all_entities_by_tag(manager, BULLET);
    list_t *enemies = entitymanager_get_all_entities_by_tag(manager, ENEMY);

    for (u64 i = 0; i < enemies->len; i++)
    {
        entity_t *enemy = (entity_t *)list_get_element_by_index(enemies, i);
        assert(enemy);
        c_boxcollider2d_t *ebox = (c_boxcollider2d_t *)entity_get_component(enemy, c_boxcollider2d_t );
        if (ebox == NULL) eprint("enemy entity is missing a box collider");

        for (u64 j = 0; j < bullets->len; j++)
        {
            entity_t *bullet = (entity_t *)list_get_element_by_index(bullets, j);
            assert(bullet);
            c_boxcollider2d_t *bbox = (c_boxcollider2d_t *)entity_get_component(bullet, c_boxcollider2d_t );
            if (bbox == NULL) eprint("bullet entity is missing a box collider");

            if (collision2d_check_collision_by_AABB(ebox, bbox))
            {
                entity_destroy(bullet);
                entity_destroy(enemy);
            }

        }
    }

    for (u64 i = 0; i < enemies->len; i++)
    {
        entity_t *enemy = (entity_t *)list_get_element_by_index(enemies, i);
        assert(enemy);
        c_boxcollider2d_t *ebox = (c_boxcollider2d_t *)entity_get_component(enemy, c_boxcollider2d_t );
        if (ebox == NULL) eprint("enemy entity is missing a box collider");

        if (collision2d_check_collision_by_AABB(ebox, pbox))
        {
            entity_destroy(enemy);
            entity_destroy(player);
        }

    }
}

void game_system_player_input(entitymanager_t *manager, entity_t *player)
{
    assert(manager);
    assert(player);

    vec3f_t playerpos = ((c_transform_t *)entity_get_component(player, c_transform_t ))->position;

    c_input_t *input = (c_input_t *)entity_get_component(player, c_input_t );
    assert(input);

    window_t *win = input->win; assert(win);

    if (window_mouse_button_just_pressed(win)) {

        entity_t *bullet = entitymanager_add_entity(manager, BULLET);
        assert(bullet);

        vec2f_t mousepos    = window_mouse_get_norm_position(win);


        vec2f_t distancevec = vec2f_sub(
                mousepos, 
                (vec2f_t ){playerpos.cmp[X], playerpos.cmp[Y]});

        f32 distance = vec2f_distance(
                mousepos, 
                (vec2f_t ){playerpos.cmp[X], playerpos.cmp[Y]});

        f32 theta = atan2(distancevec.cmp[Y], distancevec.cmp[X]);

        c_transform_t *t = c_transform_init(
                playerpos,
                MAX_BULLET_SPEED, 
                theta);
        assert(t);

        c_shape2d_t *shape = c_shape2d_init(t, CIRCLE, BULLET_SIDE, vec4f(0.3f));
        assert(shape);

        c_boxcollider2d_t *box = c_boxcollider2d_init(t, BULLET_SIDE);
        assert(box);

        c_shader_t *shader = c_shader_init("./res/player.vs", "./res/player.fs");
        assert(shader);

        entity_add_component(bullet, t, c_transform_t );
        entity_add_component(bullet, shape, c_shape2d_t );
        entity_add_component(bullet, box, c_boxcollider2d_t );
        entity_add_component(bullet, shader, c_shader_t );
    }
}


void game_system_enemy_spawner(entitymanager_t *manager)
{
    assert(manager);


}
