#pragma once
#include <poglib/poggen.h>

void menu_init(scene_t *scene)
{
    scene_register_action(scene, action("JUMP", SDLK_w));
}

void menu_update(scene_t *scene)
{
}

void menu_input(const action_t action)
{
    printf("Action = %s\n", action.label);
}

void menu_render(scene_t *scene)
{
}

void menu_destroy(scene_t *scene)
{
}
