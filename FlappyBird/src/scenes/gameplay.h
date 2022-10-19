#pragma once
#include <poglib/poggen.h>

void gameplay_init(scene_t *scene) 
{
}

void gameplay_update(scene_t *scene) 
{
}

void gameplay_input(const action_t action) {}

void gameplay_render(scene_t *scene) 
{
    poggen_t *pog = scene_get_engine();

    glrenderer2d_draw_quad(
            &(glrenderer2d_t ) {
                .shader     = assetmanager_get_shader(&pog->assets, "shader"),
                .texture    = assetmanager_get_texture2d(&pog->assets, "ground")
            }, 
            glquad(quadf((vec3f_t ){-1.0f, -0.8f, 0.0f}, 2.0f, 0.2f), COLOR_NEUTRAL, quadf(vec3f(0.0f), 0.4f, 1.0f), 0)
    );

    glrenderer2d_draw_quad(
            &(glrenderer2d_t ) {
                .shader     = assetmanager_get_shader(&pog->assets, "shader"),
                .texture    = assetmanager_get_texture2d(&pog->assets, "background")
            }, 
            glquad(quadf((vec3f_t ){-1.0f, 1.0f, 0.0f}, 2.0f, 2.0f), COLOR_NEUTRAL, quadf(vec3f(0.0f), 0.3f, 1.0f), 0)
    );


}

void gameplay_destroy(scene_t *scene) {}
