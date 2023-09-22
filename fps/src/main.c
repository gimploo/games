#define WINDOW_SDL
//#define SDL2_ENABLE_LOG
#include <poglib/application.h>
#include <poglib/poggen.h>

#include "scenes/menu.h"

#define WINDOW_WIDTH    1080
#define WINDOW_HEIGHT   920

//TODO: experiment with 3d rotation learned for chillis video
//TODO: experiment with wrap angle as well

typedef struct content_t {

    poggen_t *engine;

} content_t ;


void fps_init(application_t *app) 
{
    content_t c = {
        .engine = poggen_init()
    };

    poggen_add_scene(c.engine, menu);
    poggen_add_scene(c.engine, 3drotation);

    application_pass_content(app, &c);
}

void fps_update(application_t *app) 
{
    window_t *win = application_get_window(app);
    content_t *c = application_get_content(app);
    const f32 dt = application_get_dt(app);

    window_update_user_input(win);

    poggen_update(c->engine, dt);
}

void fps_render(application_t *app) 
{
    window_t *win = application_get_window(app);
    content_t *c = application_get_content(app);

    poggen_render(c->engine);
}

void fps_destroy(application_t *app) 
{
    window_t *win = application_get_window(app);
    content_t *c = application_get_content(app);

    poggen_destroy(c->engine);
}

int main(void)
{
    application_t app = {
        .window = {
            .title = "fps",
            .width = WINDOW_WIDTH,
            .height = WINDOW_HEIGHT,
            .aspect_ratio = (f32)WINDOW_WIDTH / (f32)WINDOW_HEIGHT,
            .fps_limit = 60,
            .background_color = COLOR_WHITE
        },   
        .content = {
            .size = sizeof(content_t )
        },
        .init       = fps_init,
        .update     = fps_update,
        .render     = fps_render,
        .destroy    = fps_destroy
    };

    application_run(&app);

    return 0;
}

