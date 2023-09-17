#pragma once
#include <poglib/poggen.h>
#include <poglib/crapgui.h>

#include <poglib/crapgui.h>


void menu_init(scene_t *scene)
{
    printf("Running menu scene\n");

    crapgui_t menu_gui = crapgui_init();

    crapgui_layout(&menu_gui) {
        frame("MENU", DEFAULT_FRAME_STYLE) {
            label("MENU SCREEN", DEFAULT_LABEL_STYLE);
            button("START", DEFAULT_BUTTON_STYLE);
        }
    }

    scene_pass_content(scene, &menu_gui, sizeof(crapgui_t));
}

void menu_input(scene_t *scene, const action_t action)
{
}

void menu_update(scene_t *scene)
{
    crapgui_t *menu_gui = (crapgui_t *)scene->content;
    crapgui_update(menu_gui);
}

void menu_render(scene_t *scene)
{
    crapgui_t *menu_gui = (crapgui_t *)scene->content;
    crapgui_render(menu_gui);
}

void menu_destroy(scene_t *scene)
{
    crapgui_t *menu_gui = (crapgui_t *)scene->content;
    crapgui_destroy(menu_gui);
}
