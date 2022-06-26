#include <poglib/poggen.h>
#include <poglib/crapgui.h>

local crapgui_t menu_gui;

void menu_init(scene_t *scene)
{
    menu_gui = crapgui_init();
    crapgui_layout(&menu_gui)
    {
        frame("MENU", DEFAULT_FRAME_STYLE)
        {
            label("Super Mario", DEFAULT_LABEL_STYLE);
            button("START GAME", DEFAULT_BUTTON_STYLE);
            button("QUIT GAME", DEFAULT_BUTTON_STYLE);
        }
    }

}

void menu_update(scene_t *scene)
{
    crapgui_update(&menu_gui);

    ui_t *start = crapgui_get_button(&menu_gui, "MENU", "START GAME");
    if (start->is_active)
        printf("Start the game\n");

    ui_t *quit = crapgui_get_button(&menu_gui, "MENU", "QUIT GAME");
    if (quit->is_active)
        printf("Exit the game\n");
}

void menu_input(const action_t action)
{
}

void menu_render(scene_t *scene)
{
    crapgui_render(&menu_gui);
}

void menu_destroy(scene_t *scene)
{
    crapgui_destroy(&menu_gui);
}

