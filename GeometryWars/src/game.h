#pragma once
#include "../lib/ecs/entitymanager.h"
#include "../lib/ecs/systems.h"

#define MAX_BULLET_SPEED vec3f(0.02f)
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

            // Out of screen
            //if (bbox->position->cmp[X] > 1.0f 
                    //|| bbox->position->cmp[Y] > 1.0f 
                    //|| bbox->position->cmp[X] < -1.0f 
                    //|| bbox->position->cmp[Y] < -1.0f)
            //{
                //entity_destroy(bullet);
            //}

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

void game_system_spawn_bullet(entitymanager_t *manager, entity_t *player)
{
    assert(manager);
    assert(player);

    c_transform_t *playertransform = (c_transform_t *)entity_get_component(player, c_transform_t );
    vec3f_t playerpos = playertransform->position;
    c_shape2d_t *playershape = (c_shape2d_t *)entity_get_component(player, c_shape2d_t );
    f32 playerside = playershape->radius;

    c_input_t *input = (c_input_t *)entity_get_component(player, c_input_t );
    assert(input);

    window_t *win = input->win; 
    assert(win);
    vec2f_t mousepos = window_mouse_get_norm_position(win);

    // bullet creation
    //
    entity_t *bullet = entitymanager_add_entity(manager, BULLET);
    assert(bullet);

    vec2f_t distancevec = vec2f_sub(
            mousepos, 
            (vec2f_t ){playerpos.cmp[X], playerpos.cmp[Y]});

    f32 distance = vec2f_distance(
            mousepos, 
            (vec2f_t ){playerpos.cmp[X], playerpos.cmp[Y]});

    f32 theta = atan2(distancevec.cmp[Y], distancevec.cmp[X]);

    c_transform_t *t = c_transform_init(
            vec3f_add(playerpos, (vec3f_t ){+playerside/2, -playerside/2}), 
            theta,
            MAX_BULLET_SPEED);
    assert(t);

    c_shape2d_t *shape = c_shape2d_init(t, CIRCLE, BULLET_SIDE, vec4f(1.0f));
    assert(shape);

    c_boxcollider2d_t *box = c_boxcollider2d_init(t, BULLET_SIDE);
    assert(box);

    c_shader_t *shader = c_shader_init("./res/player.vs", "./res/player.fs");
    assert(shader);

    c_lifespan_t *life = c_lifespan_init(4);
    assert(life);

    entity_add_component(bullet, t, c_transform_t );
    entity_add_component(bullet, shape, c_shape2d_t );
    entity_add_component(bullet, box, c_boxcollider2d_t );
    entity_add_component(bullet, shader, c_shader_t );
    entity_add_component(bullet, life, c_lifespan_t );
}

void game_system_player_input(entitymanager_t *manager, entity_t *player)
{    
    assert(manager);
    assert(player);

    c_input_t *input = (c_input_t *)entity_get_component(player, c_input_t );
    assert(input);

    window_t *win = input->win; 
    assert(win);

    if (window_mouse_button_just_pressed(win)) 
        game_system_spawn_bullet(manager, player);

}

void game_system_entity_bullet_update(entitymanager_t *manager)
{
    list_t *bullets = entitymanager_get_all_entities_by_tag(manager, BULLET);

    for (u64 i = 0; i < bullets->len; i++)
    {
        entity_t *e = *(entity_t **)list_get_element_by_index(bullets, i);

        // components
        c_transform_t *transform        = (c_transform_t *)entity_get_component(e, c_transform_t );
        c_shape2d_t *shape              = (c_shape2d_t *)entity_get_component(e, c_shape2d_t );
        c_boxcollider2d_t *collider     = (c_boxcollider2d_t *)entity_get_component(e, c_boxcollider2d_t );
        c_lifespan_t *lifespan          = (c_lifespan_t *)entity_get_component(e, c_lifespan_t );

        // updates
        lifespan->update(lifespan);
        transform->update(transform);
        shape->update(shape);
        collider->update(collider);

        //
        if(!lifespan->is_alive) 
            entity_destroy(e);
        else 
            shape->fill.cmp[3] -= 0.3f;
    }
}

void game_system_enemy_spawner(entitymanager_t *manager)
{
    assert(manager);


}
