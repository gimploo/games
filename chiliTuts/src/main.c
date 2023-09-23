#define WINDOW_SDL
#include <poglib/application.h>
#include <poglib/poggen.h>

//scenes
#include "scenes/tut03.h"
#include "scenes/tut04.h"

typedef struct content_t {

    poggen_t *engine;

} content_t ;

void chiliTuts_init(application_t *app) 
{
    content_t c = {
        .engine = poggen_init()
    };

    poggen_add_scene(c.engine, tut03);
    poggen_add_scene(c.engine, tut04);

    application_pass_content(app, &c);
}

void chiliTuts_update(application_t *app) 
{
    window_t *win = application_get_window(app);
    content_t *c = application_get_content(app);

    window_update_user_input(win);
    poggen_update(c->engine, application_get_dt(app));
}

void chiliTuts_render(application_t *app) 
{
    window_t *win = application_get_window(app);
    content_t *c = application_get_content(app);

    poggen_render(c->engine);
}

void chiliTuts_destroy(application_t *app) 
{
    window_t *win = application_get_window(app);
    content_t *c = application_get_content(app);

    poggen_destroy(c->engine);
}

int main(void)
{
    application_t app = {
        .window = {
            .title              = "chiliTuts",
            .width              = 1080,
            .height             = 920,
            .aspect_ratio       = 1080.f / 920.f,
            .fps_limit          = 60,
            .background_color   = COLOR_WHITE
        },   
        .content = {
            .size = sizeof(content_t )
        },
        .init       = chiliTuts_init,
        .update     = chiliTuts_update,
        .render     = chiliTuts_render,
        .destroy    = chiliTuts_destroy
    };

    application_run(&app);

    return 0;
}

