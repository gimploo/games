#pragma once
#include <poglib/poggen.h>
#include <poglib/util/glcamera.h>

typedef struct {

    glcamera_t camera;
    glshader_t shader;

    struct {
        vec3f_t pos;
        vec3f_t quaternion;
        matrix4f_t view;
        vec3f_t delta;
        struct {
            slot_t vtx;
            slot_t idx;
        } gfx;
    } player;

    struct {
        vec2i_t sprite_count;
        gltexture2d_t texture;
    } atlas;

    struct {
        slot_t chunks;
        slot_t uvs;
    } world;

    struct {
        list_t buffer; // pos(xyz) | uv
        list_t idx;
    } gfx;

    bool debug;

} playscene_t ;

