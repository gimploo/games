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

    vec3f_t             pos;
    f32                 jump_factor;
    f32                 gravity;
    const glshader_t    *shader;
    quadf_t             bird_rect;

    struct {
        const f32 scroll;
        const f32 width;
        const f32 height;
        const gltexture2d_t *image;
        f32                 timer;
        list_t              positions;
        list_t              uvs;
        struct {
            queue_t             rects;
            glbatch_t           batch;
        } __glcache;
    } pipes;

} sgameplay_t;

void pipes_update(sgameplay_t *game, const f32 dt)
{
    list_t *pipes = &game->pipes.positions;

    u32 indexs[20] = {0};
    i32 count = -1;

    queue_clear(&game->pipes.__glcache.rects); 
    list_iterator(pipes, iter)
    {
        vec2f_t *vec = (vec2f_t *)iter;
        vec->x += -game->pipes.scroll * dt;

        if (vec->x < -(1.0f + game->pipes.width)) {
            indexs[++count] = (u64)index;
            continue;
        }

        const quadf_t quad = quadf(
                                (vec3f_t ) {
                                    vec->x , 
                                    vec->y, 
                                    0.0f
                                }, 
                                game->pipes.width, 
                                vec->y < 0.0f ? 
                                    -1.0f * (-0.9f - vec->y) : -1.0f * (1.0f - vec->y));
        queue_put(&game->pipes.__glcache.rects, quad);
    }

    for (int i = 0; i < count; i++)
        list_delete(&game->pipes.positions, indexs[i]);


    glbatch_clear(&game->pipes.__glcache.batch);
    queue_iterator(&game->pipes.__glcache.rects, iter)
    {
        glquad_t quad = glquad(
                            *(quadf_t *)iter, 
                            COLOR_NEUTRAL, 
                            quadf(vec3f(0.0f), 1.0f, 1.0f)
                        );
        glbatch_put(&game->pipes.__glcache.batch, quad);
    }
}

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

        .shader = assetmanager_get_shader(
                    &scene_get_engine()->assets, "shader"),
        .bird_rect = quadf((vec3f_t ){-0.8f, 0.0f, 0.0f}, 0.6f, 0.4f),
        .pipes = {
            .scroll = 0.2f,
            .width = 0.3f,
            .height = 0.6f,
            .image = assetmanager_get_texture2d(
                    &scene_get_engine()->assets, "pipe"),
            .timer = 0,
            .positions = list_init(vec2f_t ),
            .uvs       = list_init(quadf_t ),
            .__glcache = {

                .rects = queue_init(10, quadf_t ),
                .batch = glbatch_init(10, glquad_t ) 
            }
        }
    };
    scene_pass_content(scene, &c, sizeof(c));

    scene_register_action(
            scene, 
            action("JUMP", ACTION_TYPE_JUSTPRESSED, SDLK_SPACE)); 

}


void gameplay_update(scene_t *scene) 
{
    sgameplay_t *c = (sgameplay_t *)scene->content;
    const f32 dt = scene->dt;
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

    // pipes
    {
        c->pipes.timer += dt;
        static bool isdown = false;
        if (c->pipes.timer > 2)
        {
            isdown = !isdown;
            const f32 val = (f32) randint(10, 40) / 100.0f;

            vec2f_t vec = {0};
            quadf_t uv = {0};
            if (isdown)    {
                vec = (vec2f_t ){ 1.0f, -val};
            } else {
                vec = (vec2f_t ) { 1.0f, val};
            }

            list_append(&c->pipes.positions, vec);
            c->pipes.timer = 0;
        }
        pipes_update(c, dt);
    }

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

    if (!glbatch_is_empty(&c->pipes.__glcache.batch))
        glrenderer2d_draw_from_batch(
            &(glrenderer2d_t ) {
                .shader = c->shader,
                .texture = c->pipes.image
            },
            &c->pipes.__glcache.batch
        );

    glrenderer2d_draw_quad(
        &(glrenderer2d_t ) {
            .shader     = c->shader,
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
            .shader     = c->shader,
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
            .shader     = c->shader,
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

    glbatch_destroy(&c->pipes.__glcache.batch);
    queue_destroy(&c->pipes.__glcache.rects);
    list_destroy(&c->pipes.positions);
    list_destroy(&c->pipes.uvs);
}
