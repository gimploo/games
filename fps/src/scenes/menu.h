#pragma once
#include <poglib/poggen.h>
#include <poglib/gui.h>

typedef struct menu_scene_t {
    gui_t menu;
} menu_scene_t ;

void menu_init(scene_t *scene)
{
    menu_scene_t c = {
        .menu = gui_init(),
    };

    gui_label(&c.menu, "MENU SCREEN", NULL);
    gui_button(&c.menu, "PLAY", NULL);

    scene_pass_content(scene, &c, sizeof(menu_scene_t ));
}

void menu_input(scene_t *scene, const action_t action)
{
}

void menu_update(scene_t *scene)
{
    gui_t *menu = scene->content;

    gui_update(menu);

    uibutton_t *play = gui_get_button(menu, "BUTTON");
    if (play->status.is_clicked) {
        printf("play button is pressed\n");
    }
}

void menu_render(scene_t *scene)
{
    gui_t *menu = scene->content;
    gui_render(menu);
}

void menu_destroy(scene_t *scene)
{
    gui_t *menu = scene->content;
    gui_destroy(menu);
}
