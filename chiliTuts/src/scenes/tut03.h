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
    bool isThetaChanged;

} tut03;

void tut03_init(scene_t *scene) 
{
    tut03 c = {
        .shader = glshader_from_cstr_init(TUT03_CUBE_VSHADER, TUT03_CUBE_FSHADER),
        .cube = glmesh_cube_init(),
        .theta = (vec3f_t ){0.0f, 0.0f, 0.0f},
        .isThetaChanged = false
    };

    scene_pass_content(scene, &c, sizeof(tut03));
}

void tut03_input(scene_t *scene, const f32 dt) 
{
    tut03 *c = scene->content;
    const f32 dtheta = PI;
    c->isThetaChanged = false;
    c->theta = (vec3f_t ){0};

    if (window_keyboard_is_key_pressed(global_window, SDLK_a)) {
        c->theta.x += wrap_angle(dtheta * dt);        
        c->isThetaChanged = true;
    }

    if (window_keyboard_is_key_pressed(global_window, SDLK_s)) {
        c->theta.y += wrap_angle(dtheta * dt);        
        c->isThetaChanged = true;
    }

    if (window_keyboard_is_key_pressed(global_window, SDLK_d)) {
        c->theta.z += wrap_angle(dtheta * dt);        
        c->isThetaChanged = true;
    }

}

void tut03_update(scene_t *scene) 
{
    tut03 *c = scene->content;

    if (!c->isThetaChanged) return;

    matrix4f_t rot  = matrix4f_multiply(
            matrix4f_rot((c->theta.x), (vec3f_t ){1.0f, 0.0f, 0.0f}),
            matrix4f_multiply(
                matrix4f_rot((c->theta.y), (vec3f_t ){0.0f, 1.0f, 0.0f}), 
                matrix4f_rot((c->theta.z), (vec3f_t ){0.0f, 0.0f, 1.0f})
            )); 

    slot_iterator(&c->cube.vtx, iter)
    {
        vec3f_t *vec = iter;
        vec4f_t tmp = glms_mat4_mulv(rot, (vec4f_t ){vec->x, vec->y, vec->z, 1.0f});
        *vec = (vec3f_t ) {
            tmp.x, tmp.y, tmp.z 
        };
    }

}

void tut03_render(scene_t *scene) 
{
    const tut03 *c = scene->content;

    glrenderer3d_draw_mesh(&(glrenderer3d_t ) {
            .shader = &c->shader,
            .textures = {0}
    }, &c->cube);

}

void tut03_destroy(scene_t *scene) 
{
    tut03 *c = scene->content;
    glshader_destroy(&c->shader);
    glmesh_destroy(&c->cube);
}
