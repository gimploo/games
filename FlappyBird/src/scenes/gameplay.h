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


} sgameplay_t;

void gameplay_init(scene_t *scene) 
{
    const sgameplay_t c = {
        .scroll = {
            .speed = {
                .bg = 0.02f,
                .fg = 0.04f
            },
            .value = {0.0f}
        },
    };
    scene_pass_content(scene, &c, sizeof(c));
}

void gameplay_update(scene_t *scene, const f32 dt) 
{
    sgameplay_t *c = (sgameplay_t *)scene->content;
    assert(c);

    c->scroll.value.bg = c->scroll.value.bg + c->scroll.speed.bg * dt;
    if (c->scroll.value.bg > 1.0f)
        c->scroll.value.bg = 0.0f;

    c->scroll.value.fg = c->scroll.value.fg + c->scroll.speed.fg * dt;
    if (c->scroll.value.fg > 1.0f)
        c->scroll.value.fg = 0.0f;

}

void gameplay_input(const action_t action) {}

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
            quadf(vec3f(0.0f), 0.6f, 0.4f), 
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
