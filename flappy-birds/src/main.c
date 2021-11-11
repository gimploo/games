#include "../lib/simple/gl_renderer2d.h"
#include "../lib/application.h"
#include "../lib/game/2D/collision.h"

typedef enum {

    GAME_MENU,
    GAME_START,
    GAME_PAUSE,
    GAME_PLAYING,
    GAME_WON,
    GAME_LOST,
    GAME_OVER,
    COUNT

} game_state_type;

typedef struct bird_t {

    vec2f_t          position;
    f32              speed;

    boxcollider2d_t collider;

    quadf_t         __vertices;
    quadf_t         __texcoord;

} bird_t;

typedef struct game_t {

    bird_t          player;
    game_state_type state;

} game_t;

void init(application_t *app )
{

}

void update(application_t *app)
{
    f32 fps = application_get_fps(app);
    f32 dt = application_get_dt(app);

}

void render(application_t *app)
{
    window_t *win = application_get_whandle(app);

    window_gl_render_begin(win);


    window_gl_render_end(win);
}

int main(void)
{
    window_t win = window_init("Flappy Birds", 700, 800, SDL_INIT_VIDEO);
    gl_shader_t shader = gl_shader_from_file_init("../res/shader.vs", "../res/shader.fs");
    gl_texture2d_t texture = gl_texture2d_init("../res/flappy-birds-texture-atlas.png");
    gl_renderer2d_t renderer = gl_renderer2d_init(&shader, &texture);

    application_t app = application_init(&win, &renderer);

    app = (application_t ){
        .init = init,
        .update = update,
        .render = render
    };

    application_run(&app);


// Cleanup
    window_destroy(&win);
    gl_renderer2d_destroy(&renderer);
    gl_shader_destroy(&shader);
    gl_texture2d_destroy(&texture);

}
