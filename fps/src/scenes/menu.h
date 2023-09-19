#pragma once
#include <poglib/poggen.h>
#include <poglib/gui.h>

gui_t menu;

void menu_init(scene_t *scene)
{
    menu = gui_init();

    gui_label(&menu, "MENU SCREEN", NULL);
    gui_button(&menu, "PLAY", NULL);
}

void menu_input(scene_t *scene, const action_t action)
{
}

void menu_update(scene_t *scene)
{
    gui_update(&menu);

    uibutton_t *play = gui_get_button(&menu, "BUTTON");
    if (play->status.is_clicked) {
        printf("play button is pressed\n");
        printf("--------------------\n");
    }
}

void menu_render(scene_t *scene)
{
    gui_render(&menu);
}

void menu_destroy(scene_t *scene)
{
    gui_destroy(&menu);
}
