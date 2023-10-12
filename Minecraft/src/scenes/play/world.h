#pragma once

#include "common.h"

const char *PLAYSCENE_VSSHADER = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 vertexPosition;\n"
    "layout (location = 1) in vec2 vertexUV;\n"

    "out vec2 uv;\n"

    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"

    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * vec4(vertexPosition, 1.0);\n"
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

typedef enum {

    FRONT_FACE = 0,
    BACK_FACE = 1,
    LEFT_FACE = 2,
    RIGHT_FACE = 3,
    TOP_FACE = 4,
    BOTTOM_FACE = 5,

} cube_faces_t;


#define CHUNK_WIDTH     16
#define CHUNK_HEIGHT    16
#define CHUNK_DEPTH     16

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

void add_block(playscene_t *scene, list_t *chunk, block_type_t type, vec3f_t nextpos, const bool isopaque[6])
{
    const cube_uv_t uv = *(cube_uv_t *)slot_get_value(
            &scene->world.uvs, 
            type);

    for (u32 face_index = 0; face_index < 6;)
    {
        const vec3f_t *pos_buffer = (vec3f_t *)DEFAULT_CUBE_VERTICES_24;
        const vec2f_t *uv_buffer = (vec2f_t *)uv.data;

        // skips a face
        if (!isopaque[face_index]){
            face_index++;
            continue;
        }

        //adding idx
        for (u32 idx = 0, offset = scene->gfx.buffer.len; idx < 6; idx++)
        {
            const u32 indice = offset + DEFAULT_CUBE_INDICES_24[idx];
            list_append(
                &scene->gfx.idx,
                indice);
        }
        
        // adding vtx
        for (u32 vtx = 0; vtx < 4; vtx++) 
        {
            const vec3f_t pos = glms_vec3_add(
                                    pos_buffer[(face_index * 4) + vtx], 
                                    nextpos);
            const vec2f_t uv = uv_buffer[(face_index * 4) + vtx];
            list_append(
                &scene->gfx.buffer, 
                ((f32 [5] ) {
                    pos.x, pos.y, pos.z, uv.u, uv.v
                }));

        }

        face_index++;
    }

    list_append(
        chunk, 
        ((block_t ) {
             .transform = nextpos,
             .type = type,
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
                front   = back = left = right = sprite_uv(sprite_count, BST_GROUND_SIDE);
                top     = sprite_uv(sprite_count, BST_GROUND_UP);
                bottom  = sprite_uv(sprite_count, BST_DIRT);
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

void generate_chunk(playscene_t *scene, list_t *chunk, const vec3f_t pos)
{
    for (u8 y = 0; y < CHUNK_HEIGHT; y++)
    {
        for (u8 z = 0; z < CHUNK_DEPTH; z++)
        {
            for (u8 x = 0; x < CHUNK_WIDTH; x++)
            {
                vec3f_t next_pos = {
                    .x = x * (BLOCK_WIDTH / 2),
                    .y = -y * (BLOCK_HEIGHT / 2),
                    .z = z * (BLOCK_WIDTH / 2)
                };

                next_pos = glms_vec3_add(next_pos, pos);

                bool isopaque[6] = {0};
                block_type_t type = 0;

                if ((z + 1) == CHUNK_DEPTH)    isopaque[FRONT_FACE] = true;
                if (z == 0)                    isopaque[BACK_FACE] = true;
                if (x == 0)                    isopaque[LEFT_FACE] = true;
                if ((x + 1) == CHUNK_WIDTH)    isopaque[RIGHT_FACE] = true;
                if (y == 0)                    isopaque[TOP_FACE] = true;
                if ((y + 1) == CHUNK_HEIGHT)   isopaque[BOTTOM_FACE] = true;
                
                //levels
                if (y == 0)                     type = BT_GROUND;
                else if (y > 0 && y < 10)       type = BT_DIRT;
                else                            type = BT_COBBLESTONE;

                add_block(scene, chunk, type, next_pos, isopaque);
            }
        }
    }

}

void generate_world(playscene_t *c)
{
    const bool full_opaque[6] = {true, true, true, true, true, true };

    list_t *chunk = slot_get_value(&c->world.chunks, 0);
    add_block(
        c, chunk,
        BT_COBBLESTONE,
        (vec3f_t ){4 * BLOCK_WIDTH/2 , BLOCK_WIDTH/2, 0.0f}, full_opaque);
    add_block(
        c, chunk,
        BT_DIRT,
        (vec3f_t ){2 * BLOCK_WIDTH/2 , BLOCK_WIDTH/2, 0.0f}, full_opaque);
    add_block(
        c, chunk,
        BT_GROUND,
        (vec3f_t ){0.0f, BLOCK_WIDTH/2, 0.0f}, full_opaque);

    vec3f_t offset = {0};
    slot_iterator(&c->world.chunks, chunk)
    {
        generate_chunk(
            c, 
            chunk,
            (vec3f_t ) {
                .x = CHUNK_WIDTH * offset.x,
                .y = -CHUNK_HEIGHT * offset.y,
                .z = CHUNK_DEPTH * offset.z
            }
        );

        offset.x++;
        if (offset.x == (u32)(c->world.chunks.len / 2)) {
            offset.x = 0;
            offset.z++;
        }
    }
}

