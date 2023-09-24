#pragma once
#include <poglib/poggen.h>

const char * const TUT04_CUBE_VSHADER = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 v_pos;\n"
    "void main()\n"
    "{\n"
        "gl_Position = vec4(v_pos, 1.0f);\n"
    "}";

const char * const TUT04_CUBE_FSHADER= 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
        "FragColor = vec4(0.0f, 0.7f, 0.0f, 1.0f);\n"
        "\n"
    "}";

typedef struct {

    glshader_t  shader;
    glmesh_t    cube;
    vec3f_t     theta;
    f32         offsetz;
    f32         offsetx;
    f32         offsety;

    slot_t  vtx;

} tut04;

void tut04_init(scene_t *scene)
{
    tut04 c = {
        .shader = glshader_from_cstr_init(TUT04_CUBE_VSHADER, TUT04_CUBE_FSHADER),
        .cube = glmesh_cube_init(),
        .theta = (vec3f_t ){0.0f, 0.0f, 0.0f},
        .offsetz = 0.0f,
        .offsety = 0.0f,
        .offsetx = 0.0f,
        .vtx = slot_clone(&c.cube.vtx),
    };

    scene_pass_content(scene, &c, sizeof(tut04));
}

void tut04_input(scene_t *scene, const f32 dt)
{
    tut04 *c = scene->content;
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

    if (window_keyboard_is_key_pressed(global_window, SDLK_j)) {
        c->offsety -= 2.0f * dt;
    }

    if (window_keyboard_is_key_pressed(global_window, SDLK_k)) {
        c->offsety += 2.0f * dt;
    }

    if (window_keyboard_is_key_pressed(global_window, SDLK_h)) {
        c->offsetx -= 2.0f * dt;
    }

    if (window_keyboard_is_key_pressed(global_window, SDLK_l)) {
        c->offsetx += 2.0f * dt;
    }

    if (window_keyboard_is_key_pressed(global_window, SDLK_r)) {
        c->offsetz += 0.8f * dt;
    }

    if (window_keyboard_is_key_pressed(global_window, SDLK_f)) {
        c->offsetz -= 0.8f * dt;
    }
}

void tut04_update(scene_t *scene)
{
    tut04 *c = scene->content;

    const matrix4f_t rot  = matrix4f_multiply(
            matrix4f_rot((c->theta.x), (vec3f_t ){1.0f, 0.0f, 0.0f}),
            matrix4f_multiply(
                matrix4f_rot((c->theta.y), (vec3f_t ){0.0f, 1.0f, 0.0f}),
                matrix4f_rot((c->theta.z), (vec3f_t ){0.0f, 0.0f, 1.0f})
            )); 

    const matrix4f_t proj = glms_perspective(
            radians(70), 
            global_poggen->handle.app->window.aspect_ratio,
            1.0f, 
            100.0f);

    slot_clear(&c->vtx);
    slot_iterator(&c->cube.vtx, iter)
    {
        vec4f_t vec = { ((vec3f_t *)iter)->x,
            ((vec3f_t *)iter)->y,
            ((vec3f_t *)iter)->z,
            1.0f 
        };

        //scale 
        vec = glms_vec4_scale(vec, 0.4f);

        //rotation
        vec = glms_mat4_mulv(rot, vec);

        //translation
        vec = glms_vec4_add(
                vec, 
                (vec4f_t ){ c->offsetx, c->offsety, c->offsetz, 0.0f});

        //proj
        vec = glms_mat4_mulv(proj, vec);

        slot_append(&c->vtx, *((vec3f_t *)&vec));
    }
}

void tut04_render(scene_t *scene)
{
    const tut04 *c = scene->content;

    glrenderer3d_draw_mesh(&(glrenderer3d_t ) {
            .shader = &c->shader,
            .textures = {0}
    }, &(glmesh_t ) {
        .vtx = c->vtx,
        .idx = c->cube.idx
    });
}

void tut04_destroy(scene_t *scene)
{
    tut04 *c = scene->content;
    glshader_destroy(&c->shader);
    glmesh_destroy(&c->cube);
    slot_destroy(&c->vtx);
}
