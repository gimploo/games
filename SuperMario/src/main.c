#include "../lib/application.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 700

void app_init(application_t *app)
{
}

void app_update(application_t *app)
{
}

void app_render(application_t *app)
{
}

void app_destroy(application_t *app)
{
}

int main(void)
{
    application_t app = {

        .window_width   = WINDOW_WIDTH,
        .window_height  = WINDOW_HEIGHT,
        .window_title   = "SuperMario",

        .game       = NULL,

        .init       = app_init,
        .update     = app_update,
        .render     = app_render,
        .shutdown   = app_destroy
    };

    application_run(&app);

    return 0;
}
