#pragma once
#include <poglib/poggen.h>
#include <poglib/util/glcamera.h>

const char *PLAYSCENE_VSSHADER = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 vertexPosition;\n"
    "layout (location = 1) in vec2 vertexUV;\n"
    "layout (location = 2) in vec3 transform;\n"

    "out vec2 uv;\n"

    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"

    "void main()\n"
    "{\n"
    "   vec3 pos = vertexPosition + transform;\n"
    "   gl_Position = projection * view * vec4(pos, 1.0);\n"
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
    BST_GROUND_UP        = 0, 
    BST_GROUND_SIDE      = 1,
    BST_DIRT             = 2,
    BST_COBBLESTONE      = 3,

    BST_COUNT
} block_sprite_type_t;

typedef enum {
    BT_DIRT,
    BT_GROUND,
    BT_COBBLESTONE,

    BT_COUNT,
} block_type_t;


typedef struct {

    vec3f_t transform;
    block_type_t type;

} block_t ;


typedef struct {

    glcamera_t camera;
    glshader_t shader;

    struct {
        vec2i_t sprite_count;
        gltexture2d_t texture;
    } atlas;

    struct {
        list_t blocks;
        slot_t uvs;
    } world;

    struct {
        list_t idx;
    } gfx;

} playscene_t ;


void add_block(playscene_t *scene, block_type_t type, vec3f_t nextpos)
{
    list_t *blocks = &scene->world.blocks;
    list_t *idx = &scene->gfx.idx;

    list_append_multiple(idx, DEFAULT_CUBE_INDICES_24);

    list_append(
        blocks, 
        ((block_t ) {
             .transform = nextpos,
             .type = BT_GROUND,
         })
    );
}

void generate_block_types(playscene_t *scene)
{
    slot_t *uvs = &scene->world.uvs;
    const vec2i_t sprite_count = scene->atlas.sprite_count;
    for (int i = 0; i < BT_COUNT; i++)
    {
        sprite_uv_t 
            front = {0}, 
            back = {0} ,
            left = {0},
            right = {0}, 
            top = {0}, 
            bottom = {0};

        switch(i)
        {
            case BT_GROUND:
                front = back = left = right = bottom = sprite_uv(
                                                        sprite_count, 
                                                        BST_GROUND_SIDE);
                top = sprite_uv(sprite_count, BST_GROUND_UP);
            break;
            case BT_DIRT:
                front = back = left = right = bottom = top = sprite_uv(sprite_count, BST_DIRT);
            break;
            case BT_COBBLESTONE:
                front = back = left = right = bottom = top = sprite_uv(sprite_count, BST_COBBLESTONE);
            break;
            default: eprint("unknown type");
        }

        slot_append(
            uvs, 
            ((cube_uv_t ){
                .front = front,
                .back = back,
                .left = left,
                .right = right,
                .top = top,
                .bottom = bottom
            }));
    }
}

void generate_world(playscene_t *scene)
{
    const u8 ground_width = 16;
    const u8 ground_height = 16;

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
            add_block(scene, BT_GROUND, next_pos);
        }
    }

    add_block(
        scene, 
        BT_GROUND,
        (vec3f_t ){0.0f, 2 * BLOCK_WIDTH/2, 0.0f});
}

void play_init(scene_t *scene)
{
    playscene_t c = {
        .camera = glcamera_perspective(
                (vec3f_t ){8.0f, 16.0f, 26.0f},
                (vec2f_t ){-0.4f, 4.6f}), 
        .shader = glshader_from_cstr_init(PLAYSCENE_VSSHADER, PLAYSCENE_FSSHADER),
        .world = {
            .blocks = list_init(block_t ),
            .uvs = slot_init(BT_COUNT, cube_uv_t)
        },
        .atlas = {
            .sprite_count = {16, 16},
            .texture = gltexture2d_init("res/blocks.png"),
        },
        .gfx = {
            .idx = list_init(u32),
        },
    };

    generate_block_types(&c);
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

    glrenderer3d_draw_mesh_custom(
        (glrendererconfig_t ) {

            .instance_count = c->world.blocks.len,
            .nattr = 3,
            .attr = {

                // position
                [0] = {
                    .ncmp = 3,    
                    .interleaved = {0},
                    .is_instanced = false,
                },

                //uv
                [1] = {
                    .ncmp = 2,
                    .interleaved = {0},
                    .is_instanced = false,
                },

                //transforms
                [2] = {
                    .ncmp = 3,
                    .interleaved = {
                        .offset = 0,
                        .stride = sizeof(block_t ), 
                    },
                    .is_instanced = true,
                },
            },

            .nsubbuffer = 3,
            .subbuffer = {
                [0] = {
                    .size = sizeof(DEFAULT_CUBE_VERTICES_24),
                    .data = (u8 *)DEFAULT_CUBE_VERTICES_24,
                },
                [1] = {
                    .size = sizeof(cube_uv_t),
                    .data = slot_get_value(&c->world.uvs, 1),
                },
                [2] = {
                    .size = list_get_size(&c->world.blocks),
                    .data = list_get_buffer(&c->world.blocks)
                },
            },
            .index = {
                .nmemb = c->gfx.idx.len,
                .data = list_get_buffer(&c->gfx.idx),
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

    gltexture2d_destroy(&c->atlas.texture);
    slot_destroy(&c->world.uvs);
    list_destroy(&c->world.blocks);
}
