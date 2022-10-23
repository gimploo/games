#pragma once
#include <poglib/poggen.h>

typedef struct sgameplay_t {
    struct {
        struct {
            f32 bg;
            f32 fg;
        } speed;
        struct {
            f32 bg;
            f32 fg;
        } value;
    } scroll;

    vec3f_t pos;
    f32     jump_factor;
    f32     gravity;

    matrix4f_t  bird_rect;

} sgameplay_t;

void gameplay_init(scene_t *scene) 
{
    const sgameplay_t c = {
        .scroll = {
            .speed = {
                .bg = 0.01f,
                .fg = 0.04f
            },
            .value = {0.0f}
        },
        .pos = {-0.8f, 0.0f, 0.0f},
        .jump_factor = 7.f,
        .gravity = 0.3f,

        .bird_rect = quadf_to_matrix4f(quadf((vec3f_t ){-0.8f, 0.0f, 0.0f}, 0.6f, 0.4f)),
    };
    scene_pass_content(scene, &c, sizeof(c));

    scene_register_action(
            scene, 
            action("JUMP", ACTION_KB_JUSTPRESSED, SDLK_SPACE)); 

}

void gameplay_update(scene_t *scene) 
{
    sgameplay_t *c = (sgameplay_t *)scene->content;
    f32 dt = scene->dt;
    assert(c);

    //parallex scroll
    {
        c->scroll.value.bg = c->scroll.value.bg + c->scroll.speed.bg * dt;
        if (c->scroll.value.bg > 1.0f)
            c->scroll.value.bg = 0.0f;

        c->scroll.value.fg = c->scroll.value.fg + c->scroll.speed.fg * dt;
        if (c->scroll.value.fg > 1.0f)
            c->scroll.value.fg = 0.0f;
    }

    //gravity
    {
        c->pos.y +=  -c->gravity *dt;
        if (c->pos.y < -0.62f || c->pos.y >= 1.2f)
            c->pos.y = 0.0f;
    } 

    /*animation
    {
        if (c->animation.bird_jump) {
            matrix4f_t mat = matrix4f_rotate(
                    MATRIX4F_IDENTITY, 
                    radians(10.f), 
                    (vec3f_t ){0.0f, 0.0f, 1.0f});
            
            c->bird_rect = matrix4f_multiply(
                                mat, 
                                quadf_to_matrix4f(
                                    quadf(
                                        (vec3f_t ){c->pos.x, c->pos.y, 0.0f}, 
                                        0.6f, 
                                        0.4f)));
            c->animation.bird_jump = false;
        } else {
            matrix4f_t mat = matrix4f_rotate(
                    MATRIX4F_IDENTITY, 
                    radians(10.f), 
                    (vec3f_t ){0.0f, 0.0f, -1.0f});
            c->bird_rect = matrix4f_multiply(
                                mat, 
                                quadf_to_matrix4f(
                                    quadf(
                                        (vec3f_t ){c->pos.x, c->pos.y, 0.0f}, 
                                        0.6f, 
                                        0.4f)));
        }
    }
    */

}

void gameplay_input(scene_t *scene, const action_t action) 
{
    sgameplay_t *c = (sgameplay_t *)scene->content;

    if (strcmp(action.label, "JUMP") == 0) { 
        c->pos.y += c->jump_factor * scene->dt;
    }
}

void gameplay_render(scene_t *scene) 
{
    const poggen_t *pog   = scene_get_engine();
    sgameplay_t *c  = (sgameplay_t *)scene->content;

    glrenderer2d_draw_quad(
        &(glrenderer2d_t ) {
            .shader     = assetmanager_get_shader(&pog->assets, "shader"),
            .texture    = assetmanager_get_texture2d(&pog->assets, "bird")
        },
        glquad(
            quadf((vec3f_t ){c->pos.x, c->pos.y, 0.0f}, 0.6f, 0.4f),
            COLOR_NEUTRAL,
            quadf(vec3f(0.0f), 1.0f, 1.0f) 
        )
    );

    glrenderer2d_draw_quad(
        &(glrenderer2d_t ) {
            .shader     = assetmanager_get_shader(&pog->assets, "shader"),
            .texture    = assetmanager_get_texture2d(&pog->assets, "ground")
        },
        glquad(
            quadf((vec3f_t ){-1.0f, -0.9f, 0.0f}, 2.0f, 0.1f), 
            COLOR_NEUTRAL,
            quadf((vec3f_t ){c->scroll.value.fg, 0.0f, 0.0f}, 0.4f, 1.0f)
        )
    );

    glrenderer2d_draw_quad(
        &(glrenderer2d_t ) {
            .shader     = assetmanager_get_shader(&pog->assets, "shader"),
            .texture    = assetmanager_get_texture2d(&pog->assets, "background")
        },
        glquad(
            quadf((vec3f_t ){-1.0f, 1.0f, 0.0f}, 2.0f, 2.0f), 
            COLOR_NEUTRAL, 
            quadf((vec3f_t ){c->scroll.value.bg, 0.0f, 0.0f}, 0.3f, 1.0f)
        )
    );

}

void gameplay_destroy(scene_t *scene) 
{
    sgameplay_t *c = (sgameplay_t *)scene->content;

}
