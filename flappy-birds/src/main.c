#include "../lib/simple/gl_renderer2d.h"
#include "../lib/application.h"

window_t win;
application_t game;
gl_renderer2d_t renderer;

void init(void *arg)
{
    gl_shader_t shader = gl_shader_default_init();
    renderer = gl_renderer2d_init(&shader, NULL);

    gl_vertex_t vertices[] = {

    };
    gl_batch_t batch = gl_batch_init(vertices, gl_quad_t);
}

void update(void *arg)
{
    f32 fps = application_get_fps(&game);
    f32 dt = application_get_dt(&game);

}

void render(void *arg)
{
    window_gl_render_begin(&win);

    gl_renderer2d_draw_from_batch(&renderer, &batch);

    window_gl_render_end(&win);
}

int main(void)
{
    win = window_init("Flappy Birds", 700, 800, SDL_INIT_VIDEO);
    game = application_init(&win, init, update, render);


    application_run(&game);
}
