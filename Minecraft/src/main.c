#define WINDOW_SDL
#include <poglib/application.h>
#include <poglib/poggen.h>
#include <poglib/ecs.h>

#include "scenes/play.h"

#define MINECRAFT_MAX_ENTITY_TYPES 2

typedef struct Minecraft {

    poggen_t *engine;

} Minecraft ;

void Minecraft_init(application_t *app) 
{
    Minecraft c = {
        .engine = poggen_init(app),
    };

    poggen_add_scene(c.engine, play);

    application_pass_content(app, &c);
}

void Minecraft_update(application_t *app) 
{
    Minecraft *c = application_get_content(app);

    poggen_update(c->engine, application_get_dt(app));
}

void Minecraft_render(application_t *app) 
{
    window_t *win = application_get_window(app);
    Minecraft *c = application_get_content(app);

    poggen_render(c->engine);
}

void Minecraft_destroy(application_t *app) 
{
    window_t *win = application_get_window(app);
    Minecraft *c = application_get_content(app);

    poggen_destroy(c->engine);
}

int main(void)
{
    application_t app = {
        .window = {
            .title              = "Minecraft",
            .width              = 1080,
            .height             = 920,
            .aspect_ratio       = 16.f / 9.f,
            .fps_limit          = 60,
            .background_color   = COLOR_WHITE
        },   
        .content = {
            .size = sizeof(Minecraft )
        },
        .init       = Minecraft_init,
        .update     = Minecraft_update,
        .render     = Minecraft_render,
        .destroy    = Minecraft_destroy
    };

    application_run(&app);

    return 0;
}

