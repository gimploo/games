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

} tut03;

glmesh_t glmesh_cube_init(void)
{
    const vec3f_t vertices[] = {
        // front
        (vec3f_t ) {-1.0f, -1.0f,  1.0f},
        (vec3f_t ) { 1.0f, -1.0f,  1.0f},
        (vec3f_t ) { 1.0f,  1.0f,  1.0f},
        (vec3f_t ) {-1.0f,  1.0f,  1.0f},
        // back
        (vec3f_t ) {-1.0f, -1.0f, -1.0f},
        (vec3f_t ) { 1.0f, -1.0f, -1.0f},
        (vec3f_t ) { 1.0f,  1.0f, -1.0f},
        (vec3f_t ) {-1.0f,  1.0f, -1.0f},
    };

      const u32 ibo[] = {
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3
    };

    slot_t vs = slot_init(ARRAY_LEN(vertices), vec3f_t);
    slot_t is = slot_init(ARRAY_LEN(ibo), u32);

    for (int i = 0; i < ARRAY_LEN(vertices); i++)
        slot_append(&vs, vertices[i]);

    for (int i = 0; i < ARRAY_LEN(ibo); i++)
        slot_append(&is, ibo[i]);

    return glmesh_init(vs, is);
}

void tut03_init(scene_t *scene) 
{
    tut03 c = {
        .shader = glshader_from_cstr_init(TUT03_CUBE_VSHADER, TUT03_CUBE_FSHADER),
        .cube = glmesh_cube_init(),
        .theta = (vec3f_t ){0.0f, 0.0f, 0.0f}
    };

    scene_pass_content(scene, &c, sizeof(tut03));
}

void tut03_input(scene_t *scene, const f32 dt) 
{
    tut03 *c = scene->content;
    const f32 dtheta = PI;

    if (window_keyboard_is_key_pressed(global_window, SDLK_a)) {
        c->theta.x += dtheta * dt;        
    }

    if (window_keyboard_is_key_pressed(global_window, SDLK_s)) {
        c->theta.y += dtheta * dt;        
    }

    if (window_keyboard_is_key_pressed(global_window, SDLK_d)) {
        c->theta.z += dtheta * dt;        
    }

}

void tut03_update(scene_t *scene) 
{
    tut03 *c = scene->content;

    matrix4f_t rot  = matrix4f_multiply(
                matrix4f_rot(c->theta.x, (vec3f_t ){1.0f, 0.0f, 0.0f}), 
                matrix4f_multiply(
                    matrix4f_rot(c->theta.y, (vec3f_t ){0.0f, 1.0f, 0.0f}), 
                    matrix4f_rot(c->theta.z, (vec3f_t ){0.0f, 0.0f, 1.0f})
            )); 

    // glms_mat4_print(rot, stdout);

    slot_t *vt = glmesh_getvertices(&c->cube);
    slot_iterator(vt, iter)
    {
        vec3f_t *vec = iter;
        printf("BEFORE "VEC3F_FMT"\n", vec->x, vec->y, vec->z);
        vec4f_t tmp = glms_mat4_mulv(rot, (vec4f_t ){vec->x, vec->y, vec->z, 0.0f});
        *vec = (vec3f_t ) {
            tmp.x, tmp.y, tmp.z
        };
        printf("AFTER "VEC3F_FMT"\n", vec->x, vec->y, vec->z);
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
