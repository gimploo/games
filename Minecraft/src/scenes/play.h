#pragma once

#include "play/world.h"
#include "play/player.h"

void play_init(scene_t *scene)
{
    const u32 total_chunks = 4;

    playscene_t c = {
        .camera = glcamera_perspective(
                (vec3f_t ){8.0f, 16.0f, 26.0f},
                (vec2f_t ){-0.4f, 4.6f}), 
        .shader = glshader_from_cstr_init(PLAYSCENE_VSSHADER, PLAYSCENE_FSSHADER),
        .world = {
            .chunks = slot_init(total_chunks, list_t),
            .uvs = slot_init(BT_COUNT, cube_uv_t)
        },
        .atlas = {
            .sprite_count = {16, 16},
            .texture = gltexture2d_init("res/blocks.png"),
        },
        .gfx = {
            .buffer = list_init(f32 [5]), // 1 vertex [pos[3] uv[2]]
            .idx = list_init(u32),
        },
        .player = {
            .pos = {0.0f, 2.0f, 0.0f},
            .quaternion = {0.0f, 0.0f, 0.0f},
            .view = MATRIX4F_IDENTITY,
            .gfx = {
                .vtx = slot_init(ARRAY_LEN(DEFAULT_CUBE_VERTICES_8), f32),
                .idx = slot_init(ARRAY_LEN(DEFAULT_CUBE_INDICES_8), u32),
            }
        }
    };

    for(u8 i = 0; i < total_chunks; i++) 
    {
        const list_t chunk = list_init(block_t );
        slot_insert(&c.world.chunks, i, chunk);
    }

    create_player_hitbox(&c);
    generate_block_types(&c);
    generate_world(&c);

    scene_pass_content(scene, &c, sizeof(playscene_t ));
}


void play_input(scene_t *scene, const f32 dt)
{
    playscene_t *c = scene->content;

    glcamera_process_input(&c->camera, dt);

    update_player_input(c);
}

void play_update(scene_t *scene, const f32 dt)
{
    playscene_t *c = scene->content;
    const f32 aspect_ratio = global_poggen->handle.app->window.aspect_ratio;

    glshader_send_uniform_matrix4f(
            &c->shader, 
            "projection",
            glms_perspective(
                radians(45), 
                aspect_ratio, 
                1.0f, 1000.0f
            ));

    update_player(c);
    glshader_send_uniform_matrix4f(
            &c->shader, 
            "view",
            c->player.view);

    // glshader_send_uniform_matrix4f(
    //         &c->shader, 
    //         "view",
    //         glcamera_getview(&c->camera));

}

void play_render(scene_t *scene)
{
    playscene_t *c = scene->content;

    glrenderer3d_draw(
        (glrendererconfig_t ) {

            .nattr = 3,
            .attr = {

                // position
                [0] = {
                    .buffer_index = 0,
                    .ncmp = 3,    
                    .interleaved = {
                        .offset = 0,
                        .stride = sizeof(vec3f_t ) + sizeof(vec2f_t ),
                    },
                },

                //uv
                [1] = {
                    .buffer_index = 0,
                    .ncmp = 2,
                    .interleaved = {
                        .offset = sizeof(vec3f_t),
                        .stride = sizeof(vec3f_t ) + sizeof(vec2f_t ),
                    },
                },

                // player pos
                [2] = {
                    .buffer_index = 1,
                    .ncmp = 3,    
                    .interleaved = {0},
                },

            },

            .nbuffer = 2,
            .buffer = {
                [0] = {
                    .size = list_get_size(&c->gfx.buffer),
                    .data = list_get_buffer(&c->gfx.buffer),

                    .indexbuffer = {
                        .nmemb = c->gfx.idx.len,
                        .data = list_get_buffer(&c->gfx.idx),
                    },
                },

                [1] = {
                    .size = slot_get_size(&c->player.gfx.vtx),
                    .data = slot_get_buffer(&c->player.gfx.vtx),

                    .indexbuffer = {
                        .nmemb = c->player.gfx.idx.len,
                        .data = slot_get_buffer(&c->player.gfx.idx),
                    },
                }
            },

            .shader = &c->shader,
            .ntexture = 1, 
            .texture = {
                [0] = &c->atlas.texture
            }
        }
    );
}

void play_destroy(scene_t *scene)
{
    playscene_t *c = scene->content;
    glshader_destroy(&c->shader);

    list_destroy(&c->gfx.idx);
    list_destroy(&c->gfx.buffer);

    slot_destroy(&c->player.gfx.idx);
    slot_destroy(&c->player.gfx.vtx);

    gltexture2d_destroy(&c->atlas.texture);
    slot_destroy(&c->world.uvs);

    slot_iterator(&c->world.chunks, blocks)
        list_destroy((list_t *)blocks);
    slot_destroy(&c->world.chunks);
}
