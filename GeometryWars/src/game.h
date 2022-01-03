#pragma once
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

typedef struct game_t {

    entity_t        *player;
    entitymanager_t manager;

} game_t;



void game_update_bullet_enemy_collision(entitymanager_t *manager)
{
    assert(manager);

    list_t *bullets = entitymanager_get_all_entities_by_tag(manager, GET_BULLET);
    list_t *enemies = entitymanager_get_all_entities_by_tag(manager, GET_ENEMY);

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
}

void game_update_player_enemy_collision(entitymanager_t *manager, entity_t *player)
{
    assert(manager);

    list_t *enemies = entitymanager_get_all_entities_by_tag(manager, GET_ENEMY);

    c_boxcollider2d_t *pbox = (c_boxcollider2d_t *)entity_get_component(player, c_boxcollider2d_t );
    assert(pbox);

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

void game_spawn_enemies(entitymanager_t *manager)
{
    assert(manager);

}

