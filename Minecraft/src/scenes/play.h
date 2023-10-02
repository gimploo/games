#pragma once
#include <poglib/poggen.h>
#include <poglib/util/glcamera.h>

const char *PLAYSCENE_VSSHADER = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 vertexPosition;\n"
    "layout (location = 1) in vec2 vertexUV;\n"

    "out vec2 uv;\n"

    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 transform;\n"

    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * transform * vec4(vertexPosition, 1.0);\n"
    "   uv = vertexUV;\n"
    "}\n";

const char *PLAYSCENE_FSSHADER = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 uv;\n"

    "uniform sampler2D atlas;\n"

    "void main()\n"
    "{\n"
    "       FragColor = texture(atlas, uv);\n"
    "}\n";

#define BLOCK_WIDTH     2.0f
#define BLOCK_HEIGHT    2.0f

typedef enum {
    BT_GROUND_UP        = 0, 
    BT_GROUND_SIDE      = 1,
    BT_DIRT             = 2,

    BT_COUNT
} block_type_t;

typedef struct {

    matrix4f_t transform;
    block_type_t type;

} block_t ;

typedef struct {

    glcamera_t camera;
    glshader_t shader;

    struct {
        slot_t vtx;
        slot_t idx;
    } cube;

    struct {
        vec2i_t sprite_count;
        gltexture2d_t texture;
        list_t uvs;
    } atlas;

    struct {
        list_t blocks;
    } world;

} playscene_t ;

typedef union {
    struct {
        sprite_uv_t front, back;
        sprite_uv_t left, right;
        sprite_uv_t top, bottom;
    };
} cube_uv_t;

void generate_uvs(playscene_t *scene)
{
    list_t *uvs = &scene->atlas.uvs;
    const sprite_uv_t front = sprite_uv(scene->atlas.sprite_count, 0);
    list_append(
            uvs, 
            ((cube_uv_t ){
                front, front, front, front, front, front
            }));
}

void generate_world(playscene_t *scene)
{
    const u8 ground_width = 16;
    const u8 ground_height = 16;

    list_t *blocks = &scene->world.blocks;

    for (u8 z = 0; z < ground_height; z++)
    {
        vec3f_t next_pos = vec3f(0.0f);
        for (u8 x = 0; x < ground_width; x++)
        {
            next_pos = (vec3f_t ){
                .x = x * (BLOCK_WIDTH / 2),
                .y = 0.0f,
                .z = z * (BLOCK_HEIGHT / 2)
            };

            //NOTE: stopped here
            const matrix4f_t mat = glms_translate_make(next_pos);
            list_append(
                    blocks, 
                    ((block_t ) {
                         .transform = mat,
                         .type = BT_GROUND_UP,
                     })
            );
        }
    }

    block_t block = {
        .transform = glms_translate_make((vec3f_t ){0.0f, 2 * BLOCK_WIDTH/2, 0.0f}),
        .type = BT_GROUND_UP,
    };
    list_append(blocks, block);
}

void setup_uv_cube(playscene_t *scene)
{
    slot_t *vtx = &scene->cube.vtx;
    slot_t *idx = &scene->cube.idx;

    slot_insert_multiple(vtx, DEFAULT_CUBE_VERTICES_24);
    slot_insert_multiple(idx, DEFAULT_CUBE_INDICES_24);
}

void play_init(scene_t *scene)
{
    playscene_t c = {
        .camera = glcamera_perspective(
                (vec3f_t ){8.0f, 16.0f, 26.0f},
                (vec2f_t ){-0.4f, 4.6f}), 
        .shader = glshader_from_cstr_init(PLAYSCENE_VSSHADER, PLAYSCENE_FSSHADER),
        .cube = {
            .vtx = slot_init(ARRAY_LEN(DEFAULT_CUBE_VERTICES_24), f32),
            .idx = slot_init(ARRAY_LEN(DEFAULT_CUBE_INDICES_24), u32),
        },
        .world = {
            .blocks = list_init(block_t ),
        },
        .atlas = {
            .sprite_count = {16, 16},
            .texture = gltexture2d_init("res/blocks.png"),
            .uvs = list_init(cube_uv_t)
        },
    };

    setup_uv_cube(&c);
    generate_uvs(&c);
    generate_world(&c);

    scene_pass_content(scene, &c, sizeof(playscene_t ));
}


void play_input(scene_t *scene, const f32 dt)
{
    playscene_t *c = scene->content;

    glcamera_process_input(&c->camera, dt);
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

    glshader_send_uniform_matrix4f(
            &c->shader, 
            "view",
            glcamera_getview(&c->camera));


}

void play_render(scene_t *scene)
{
    playscene_t *c = scene->content;

    list_iterator(&c->world.blocks, iter)
    {
        block_t *block = iter;

        glshader_send_uniform_matrix4f(
                &c->shader, "transform", 
                block->transform);
        
        glrenderer3d_draw_mesh_custom(
                (glrendererconfig_t ) {

                    .nattr = 2,
                    .attr = {

                        // position
                        [0] = {
                            .ncmp = 3,    
                            .stride = 0,
                            .offset = 0
                        },

                        //uv
                        [1] = {
                            .ncmp = 2,
                            .stride = 0,
                            .offset = slot_get_size(&c->cube.vtx)
                        }
                    },

                    .nsubbuffer = 2,
                    .subbuffer = {
                        [0] = {
                            .size = slot_get_size(&c->cube.vtx),
                            .data = slot_get_buffer(&c->cube.vtx),
                        },
                        [1] = {
                            .size = sizeof(cube_uv_t),
                            .data = list_get_value(&c->atlas.uvs, 0),
                        },
                    },
                    .index = {
                        .nmemb = c->cube.idx.len,
                        .data = slot_get_buffer(&c->cube.idx),
                    },

                    .shader = &c->shader,
                    .ntexture = 1, 
                    .texture = {
                        [0] = &c->atlas.texture
                    }
                }
        );
    }
}

void play_destroy(scene_t *scene)
{
    playscene_t *c = scene->content;
    glshader_destroy(&c->shader);

    gltexture2d_destroy(&c->atlas.texture);
    list_destroy(&c->atlas.uvs);
    list_destroy(&c->world.blocks);
}
