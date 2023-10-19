#pragma once
#include "common.h"

const char *SHADER_PLAYER_VS = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 vertexPosition;\n"

    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 transform;\n"

    "void main()\n"
    "{\n"
    "   gl_Position = transform * vec4(vertexPosition, 1.0);\n"
    "}\n";

const char *SHADER_PLAYER_FS = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"

    "void main()\n"
    "{\n"
    "       FragColor = vec4(0.3f, 0.2f, 0.4f, 1.0f);\n"
    "}\n";

void update_player_mesh(playscene_t *c, const vec3f_t pos, const vec3f_t theta)
{
    const matrix4f_t rot = glms_mat4_mul(
        matrix4f_rot(theta.y, (vec3f_t){0.f, 1.f, 0.f}), //y
        matrix4f_rot(theta.x, (vec3f_t){1.f, 0.f, 0.f}) //x
    ); 

    slot_iterator(&c->player.gfx.vtx, iter) {
        vec3f_t *vec = (vec3f_t *)iter;
        *vec = vec3f_cast(glms_mat4_mulv(rot, (vec4f_t ) {
                .x = vec->x * pos.x,
                .y = vec->y * pos.y,
                .z = vec->z * pos.z,
                .w = 1.0f
            }));

        glm_vec4_print(vec->raw, stdout);
    }

}

void create_player_mesh(playscene_t *c)
{
    slot_t *vtx = &c->player.gfx.vtx;
    slot_t *idx = &c->player.gfx.idx;

    vec3f_t cv[8] = {0};
    memcpy(cv, DEFAULT_CUBE_VERTICES_8, sizeof(cv));

    slot_insert_multiple(vtx, cv);
    slot_insert_multiple(idx, DEFAULT_CUBE_INDICES_8);

}

void update_player_input(playscene_t *c)
{
    const f32 dx = 10.f;
    const f32 dt = application_get_dt(global_poggen->handle.app);
    const vec3f_t PLAYER_UP     = {0.0f, 1.0f, 0.0f};

    const vec2f_t norm_rel = glms_vec2_normalize((vec2f_t ) { 
            .x = global_window->mouse.rel.x,
            .y = global_window->mouse.rel.y
        });
    const vec2f_t mouse_delta = {
        .x = wrap_angle(norm_rel.x * c->player.config.sens * dt), 
        .y = wrap_angle(norm_rel.y * c->player.config.sens * dt)
    };
    const vec4f_t player_front = glms_mat4_mulv(
            glms_mat4_mul(
                matrix4f_rot(mouse_delta.y, (vec3f_t){1.f, 0.f, 0.f}), //y
                matrix4f_rot(mouse_delta.x, (vec3f_t){0.f, 1.f, 0.f}) //x
            ), 
            (vec4f_t ) {
                c->player.direction.front.x,
                c->player.direction.front.y,
                c->player.direction.front.z,
                1.0f
            });

    c->player.direction.front = glms_normalize(
            (vec3f_t ){
                player_front.x,
                player_front.y,
                player_front.z
            });


    window_t *win = global_poggen->handle.app->handle.window;
    if (window_keyboard_is_key_pressed(win, SDLK_w))
        c->player.pos = glms_vec3_add(
                            c->player.pos, 
                            glms_vec3_scale(c->player.direction.front, dx * dt)
                        );
    if (window_keyboard_is_key_pressed(win, SDLK_a))
        c->player.pos = glms_vec3_sub(
                            c->player.pos,
                            glms_vec3_scale(
                                glms_vec3_crossn( c->player.direction.front, PLAYER_UP), 
                                dx * dt
                            ) 
                        );
    if (window_keyboard_is_key_pressed(win, SDLK_s))
        c->player.pos = glms_vec3_sub(
                            c->player.pos, 
                            glms_vec3_scale(c->player.direction.front, dx * dt)
                        );
    if (window_keyboard_is_key_pressed(win, SDLK_d))
        c->player.pos = glms_vec3_add(
                            c->player.pos,
                            glms_vec3_scale(
                                glms_vec3_crossn( c->player.direction.front, PLAYER_UP), 
                                dx * dt
                            ) 
                        );
    c->player.view = glms_lookat(
            c->player.pos, 
            glms_vec3_add(c->player.pos, c->player.direction.front), 
            PLAYER_UP);

    printf("pos   = "VEC3F_FMT"\n", VEC3F_ARG(c->player.pos));
    printf("theta = "VEC2F_FMT"\n", VEC3F_ARG(mouse_delta));

    update_player_mesh(
            c, 
            c->player.pos, 
            (vec3f_t ) {
                mouse_delta.x,
                mouse_delta.y,
                0.f
            });


}


