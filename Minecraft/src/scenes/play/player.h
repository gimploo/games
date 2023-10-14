#pragma once
#include "common.h"

void update_player(playscene_t *c, const vec3f_t delta)
{
    slot_t *vtx = &c->player.gfx.vtx;
    slot_iterator(vtx, iter)
    {
        vec3f_t *v = (vec3f_t *)iter;
        *v = (vec3f_t ){
            .x = delta.x + v->x,
            .y = delta.y + v->y,
            .z = delta.z + v->z,
        };
    }
}

void create_player_hitbox(playscene_t *c)
{
    slot_t *vtx = &c->player.gfx.vtx;
    slot_t *idx = &c->player.gfx.idx;

    vec3f_t cv[8] = {0};
    memcpy(cv, DEFAULT_CUBE_VERTICES_8, sizeof(cv));

    slot_insert_multiple(vtx, cv);
    slot_insert_multiple(idx, DEFAULT_CUBE_INDICES_8);

    update_player(c, c->player.pos);
}

void update_player_input(playscene_t *c)
{
    if (c->debug) return;

    const f32 dx = 10.f;
    const f32 dt = application_get_dt(global_poggen->handle.app);

    const vec2f_t mpos = window_mouse_get_norm_position(global_window);

    const vec3f_t PLAYER_FRONT = {mpos.x, mpos.y, -1.0f};
    const vec3f_t PLAYER_UP = {0.0f, 1.0f, 0.0f};

    const vec3f_t oldpos = c->player.pos;

    window_t *win = global_poggen->handle.app->handle.window;
    if (window_keyboard_is_key_pressed(win, SDLK_w))
        c->player.pos = glms_vec3_add(
                            c->player.pos, 
                            glms_vec3_scale(PLAYER_FRONT, dx * dt)
                        );
    if (window_keyboard_is_key_pressed(win, SDLK_a))
        c->player.pos = glms_vec3_sub(
                            c->player.pos,
                            glms_vec3_scale(
                                glms_vec3_crossn( PLAYER_FRONT, PLAYER_UP), 
                                dx * dt
                            ) 
                        );
    if (window_keyboard_is_key_pressed(win, SDLK_s))
        c->player.pos = glms_vec3_sub(
                            c->player.pos, 
                            glms_vec3_scale(PLAYER_FRONT, dx * dt)
                        );
    if (window_keyboard_is_key_pressed(win, SDLK_d))
        c->player.pos = glms_vec3_add(
                            c->player.pos,
                            glms_vec3_scale(
                                glms_vec3_crossn( PLAYER_FRONT, PLAYER_UP), 
                                dx * dt
                            ) 
                        );

    c->player.view = glms_lookat(
            c->player.pos, 
            glms_vec3_add(c->player.pos, PLAYER_FRONT), 
            PLAYER_UP);

    c->player.quaternion = (vec3f_t ){ mpos.y, mpos.x, 0.f }; 
    
    const vec3f_t newpos = c->player.pos;
    if (oldpos.x != newpos.x 
        || oldpos.y != newpos.y
        || oldpos.z != newpos.z) {

        c->player.delta = glms_vec3_sub(newpos, oldpos);
        update_player(c, c->player.delta);
    }
}


