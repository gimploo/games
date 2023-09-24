#pragma once
#include <poglib/poggen.h>

const char * const TUT03_CUBE_VSHADER = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 v_pos;\n"
    "void main()\n"
    "{\n"
        "gl_Position = vec4(v_pos, 1.0f);\n"
    "}";

const char * const TUT03_CUBE_FSHADER= 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
        "FragColor = vec4(0.7f, 0.0f, 0.0f, 1.0f);\n"
        "\n"
    "}";

typedef struct {

    glshader_t shader;
    glmesh_t cube;

    vec3f_t theta;

    slot_t vtx;

    f32 scale;

} tut03;

void tut03_init(scene_t *scene) 
{
    tut03 c = {
        .shader = glshader_from_cstr_init(TUT03_CUBE_VSHADER, TUT03_CUBE_FSHADER),
        .cube = glmesh_cube_init(),
        .theta = (vec3f_t ){0.0f, 0.0f, 0.0f},
        .vtx = slot_clone(&c.cube.vtx),
        .scale = 0.2f
    };

    scene_pass_content(scene, &c, sizeof(tut03));
}

void tut03_input(scene_t *scene, const f32 dt) 
{
    tut03 *c = scene->content;
    const f32 dtheta = PI;

    if (window_keyboard_is_key_pressed(global_window, SDLK_a)) {
        c->theta.x += wrap_angle(dtheta * dt);        
    }

    if (window_keyboard_is_key_pressed(global_window, SDLK_s)) {
        c->theta.y += wrap_angle(dtheta * dt);        
    }

    if (window_keyboard_is_key_pressed(global_window, SDLK_d)) {
        c->theta.z += wrap_angle(dtheta * dt);        
    }

    if (window_keyboard_is_key_pressed(global_window, SDLK_SPACE)) {
        c->scale += 1.0f * dt;        
    }

}

void tut03_update(scene_t *scene) 
{
    tut03 *c = scene->content;

    matrix4f_t rot  = matrix4f_multiply(
            matrix4f_rot((c->theta.x), (vec3f_t ){1.0f, 0.0f, 0.0f}),
            matrix4f_multiply(
                matrix4f_rot((c->theta.y), (vec3f_t ){0.0f, 1.0f, 0.0f}), 
                matrix4f_rot((c->theta.z), (vec3f_t ){0.0f, 0.0f, 1.0f})
            )); 

    slot_clear(&c->vtx);
    slot_iterator(&c->cube.vtx, iter)
    {
        vec3f_t *vec = iter;
        vec4f_t tmp = {0};
        tmp = glms_mat4_mulv(rot, (vec4f_t ){vec->x, vec->y, vec->z, 1.0f});
        tmp = glms_vec4_scale(tmp, c->scale);

        vec3f_t newvtx = {tmp.x, tmp.y, tmp.z};
        slot_append(&c->vtx, newvtx);
    }

}

void tut03_render(scene_t *scene) 
{
    const tut03 *c = scene->content;

    glrenderer3d_draw_mesh(&(glrenderer3d_t ) {
            .shader = &c->shader,
            .textures = {0}
    }, &(glmesh_t ) {
        .vtx = c->vtx,
        .idx = c->cube.idx
    });

}

void tut03_destroy(scene_t *scene) 
{
    tut03 *c = scene->content;
    glshader_destroy(&c->shader);
    glmesh_destroy(&c->cube);
    slot_destroy(&c->vtx);
}
