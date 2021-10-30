#include "../lib/basic.h"
#include "../lib/math/shapes.h"
#include "../lib/simple/gl_renderer2d.h"
#include "../lib/simple/window.h"
#include "../lib/simple/font/gl_ascii_font.h"
#include "../lib/game/2D/collision.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 920

typedef struct player_t {

    quadf_t padle;
    vec2f_t position;
    vec2f_t speed;

    f32 width;
    f32 height;
     
    int points;

} player_t;

typedef struct ball_t {

    quadf_t ball_quad;
    vec2f_t position;
    vec2f_t speed;

    f32 width;
    f32 height;

} ball_t;

typedef struct game_pong_t {

    enum game_state
    {
        MENU,
        PAUSE,
        PLAYING,
        PLAYER01_SCORED,
        PLAYER02_SCORED,
        PLAYER01_WON,
        PLAYER02_WON,
        EXIT,

    } prev_state, current_state;

    player_t player01, player02;
    ball_t ball;

    window_t *window;
    gl_renderer2d_t *renderer;
    const gl_ascii_font_t *font;

} game_pong_t ;

#define game_set_current_state(pgame, state) {\
    (pgame)->prev_state = (pgame)->current_state;\
    (pgame)->current_state = state;\
}


void    game_state_playing(game_pong_t *pong);
void    game_state_menu(game_pong_t *pong);
void    game_state_pause(game_pong_t *pong);
void    game_state_player_won(game_pong_t *pong);
void    game_state_exit(game_pong_t *pong);


int main(void)
{
    window_t window = window_init("pong", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_INIT_VIDEO);
    window_set_background(&window, (vec4f_t){0.0f, 0.0f, 0.0f, 0.0f});

    gl_shader_t shader          = gl_shader_from_file_init("res/pong.vs", "res/pong.fs");
    gl_renderer2d_t renderer    = gl_renderer2d_init(&shader, NULL); 

    const f32 padle_width               = 0.03f;
    const f32 padle_height              = 0.3f;
    const vec2f_t player_speed   = {0.0f, 0.03f};
    const vec2f_t ball_speed     = {0.008f, 0.001f};
    const gl_ascii_font_t font          = gl_ascii_font_init("res/charmap-futuristic_black.png",18, 7); 


    game_pong_t pong = {

        .prev_state     = MENU,
        .current_state  = MENU,

        .player01 = {
            .position = (vec2f_t) {-0.9f, 0.0f},
            .speed = player_speed,
            .width = padle_width,
            .height = padle_height,
            .points = 0,
        },

        .player02 = {
            .position   = (vec2f_t) {0.8f, 0.0f},
            .speed = player_speed,
            .width = padle_width,
            .height = padle_height,
            .points = 0,
        },

        .ball = {
            .width = 0.03f,
            .height = 0.03f,
            .position =  (vec2f_t) {-0.03f, 0.03f},
            .speed = ball_speed,
        },

        .window = &window,
        .renderer = &renderer,
        .font = &font

    };


    f64 dt;

    window_while_is_open(&window)
    {
        window_cap_fps(&window, 30);
        dt = window_get_dt(&window); // not used

        switch(pong.current_state)
        {
            case MENU:
                game_state_menu(&pong);
            break;

            case PLAYING:
                game_state_playing(&pong);
            break;

            case PLAYER01_SCORED:
                pong.ball.position  = vec2f(0.0f);
                pong.ball.speed     = ball_speed;
                pong.player01.points++;
                if (pong.player01.points == 3) {
                    game_set_current_state(&pong, PLAYER01_WON);
                } else {
                    game_state_playing(&pong);
                }
            break;

            case PLAYER02_SCORED:
                pong.ball.position      = vec2f(0.0f);
                pong.ball.speed         = ball_speed;
                pong.ball.speed.cmp[X] *= -1;
                pong.player02.points++;
                if (pong.player02.points == 3) {
                    game_set_current_state(&pong, PLAYER02_WON);
                } else {
                    game_state_playing(&pong);
                }
            break;

            case PLAYER01_WON:
                game_state_player_won(&pong);
            break;

            case PLAYER02_WON:
                game_state_player_won(&pong);
            break;

            case PAUSE:
                game_state_pause(&pong);
            break;

            case EXIT:
                game_state_exit(&pong);
            break;

            default:
                eprint("Game state type not accounted for ");
        }

    }

    gl_ascii_font_destroy(&font);
    gl_renderer2d_destroy(&renderer);
    window_destroy(&window);
    

    return 0;
}

void game_state_playing(game_pong_t *pong)
{
        ball_t ball                     = pong->ball;
        player_t player01               = pong->player01;
        player_t player02               = pong->player02;
        window_t *window                = pong->window;
        gl_renderer2d_t *renderer       = pong->renderer;
        const gl_ascii_font_t *font     = pong->font;
        const quadf_t net_quad          = quadf_init((vec2f_t ){0.0f, 1.0f}, 0.001f, 2.0f);
        const f32 ball_dx_multiplier    = 0.004;  
        const f32 ball_dy_multiplier    = 0.0001;

        f64 fps;

        window_while_is_open(window)
        {   
            fps = window_get_fps(window);
            ball.position   = vec2f_translate(ball.position, ball.speed);
            player01.padle  = quadf_init(player01.position, player01.width, player01.height);
            player02.padle  = quadf_init(player02.position, player02.width, player02.height);
            ball.ball_quad  = quadf_init(ball.position, ball.width, ball.height); 


            if (ball.position.cmp[X] >= 1.0f) {

                player01.points++;
                game_set_current_state(pong, PLAYER01_SCORED);

            } else if ( ball.position.cmp[X] <= -1.0f) {

                player02.points++;
                game_set_current_state(pong, PLAYER02_SCORED);

            }

            if (ball.position.cmp[Y] >= 1.0f 
                || ball.position.cmp[Y] <= -1.0f)
            {
                ball.speed.cmp[Y] *= -1;
            }

            if (window_keyboard_is_key_just_pressed(window, SDLK_w)) {

                player01.position = vec2f_translate(
                        player01.position, 
                        player01.speed);

                if(player01.position.cmp[Y] >= 1.0f) 
                    player01.position.cmp[Y] = 1.0f;

            } else if (window_keyboard_is_key_just_pressed(window, SDLK_s)) {

                player01.position = vec2f_translate(
                        player01.position, 
                        vec2f_scale(player01.speed, -1));

                if(player01.position.cmp[Y] <= -1.0f - (-0.3f)) 
                    player01.position.cmp[Y] = -1.0f - (-0.3f);
            }

            if (window_keyboard_is_key_just_pressed(window, SDLK_UP)) {

                player02.position = vec2f_translate(
                        player02.position, 
                        player02.speed);

                if(player02.position.cmp[Y] >= 1.0f) 
                    player02.position.cmp[Y] = 1.0f;

            } else if (window_keyboard_is_key_just_pressed(window, SDLK_DOWN)) {

                player02.position = vec2f_translate(
                        player02.position, 
                        vec2f_scale(player02.speed, -1));

                if(player02.position.cmp[Y] <= -1.0f - (-0.3f)) 
                    player02.position.cmp[Y] = -1.0f - (-0.3f);
            }

            if (window_keyboard_is_key_just_pressed(window, SDLK_ESCAPE)) {
                game_set_current_state(pong, PAUSE);
            }


            if (collision_quad_check_by_AABB(player01.padle, ball.ball_quad))
            {
                ball.speed.cmp[X] -= ball_dx_multiplier;
                ball.speed.cmp[X] *= -1;

                ball.speed.cmp[Y] += ball_dy_multiplier;
            }

            if (collision_quad_check_by_AABB(ball.ball_quad, player02.padle))
            {
                ball.speed.cmp[X] += ball_dx_multiplier;
                ball.speed.cmp[X] *= -1;

                ball.speed.cmp[Y] += ball_dy_multiplier;
            }


            window_gl_render_begin(window);
            {
                
                gl_quad_t quads[4] = {

                    // positions                                                            // colors          // texture_coord
                    ball.ball_quad.vertex[0].cmp[X], ball.ball_quad.vertex[0].cmp[Y], 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    ball.ball_quad.vertex[1].cmp[X], ball.ball_quad.vertex[1].cmp[Y], 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    ball.ball_quad.vertex[2].cmp[X], ball.ball_quad.vertex[2].cmp[Y], 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    ball.ball_quad.vertex[3].cmp[X], ball.ball_quad.vertex[3].cmp[Y], 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,

                    player01.padle.vertex[0].cmp[X], player01.padle.vertex[0].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    player01.padle.vertex[1].cmp[X], player01.padle.vertex[1].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    player01.padle.vertex[2].cmp[X], player01.padle.vertex[2].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    player01.padle.vertex[3].cmp[X], player01.padle.vertex[3].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,

                    player02.padle.vertex[0].cmp[X], player02.padle.vertex[0].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    player02.padle.vertex[1].cmp[X], player02.padle.vertex[1].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    player02.padle.vertex[2].cmp[X], player02.padle.vertex[2].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    player02.padle.vertex[3].cmp[X], player02.padle.vertex[3].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,

                    net_quad.vertex[0].cmp[X],net_quad.vertex[0].cmp[Y], 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                    net_quad.vertex[1].cmp[X],net_quad.vertex[1].cmp[Y], 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                    net_quad.vertex[2].cmp[X],net_quad.vertex[2].cmp[Y], 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                    net_quad.vertex[3].cmp[X],net_quad.vertex[3].cmp[Y], 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f

                };

                gl_batch_t batch = {
                    .shape_type= BT_QUAD, 
                    .shape_count= 4,
                    .vertex_buffer= (gl_vertex_t *)quads,
                    .vertex_buffer_size= sizeof(quads)
                };

                char string_buffer[32] = {0};

                snprintf(string_buffer, 32, "%i", player01.points);
                gl_ascii_font_render_text(font, string_buffer, (vec2f_t) {-0.6f, 1.0f}, 0.1f);

                snprintf(string_buffer, 32, "%i", player02.points);
                gl_ascii_font_render_text(font, string_buffer, (vec2f_t) {0.5f, 1.0f}, 0.1f);

                snprintf(string_buffer, 32, "FPS:%i", (int )fps);
                gl_ascii_font_render_text(pong->font, string_buffer, (vec2f_t) {0.76f, 1.0f}, 0.03f);

                gl_renderer2d_draw_from_batch(renderer, &batch); 

            }
            window_gl_render_end(window);
        }
}

void game_state_menu(game_pong_t *pong)
{
    window_gl_render_begin(pong->window); 
    {
        gl_ascii_font_render_text(
                pong->font,
                "pong",
                (vec2f_t ){-0.7f, 0.25f},
                0.35f);
        gl_ascii_font_render_text(
                pong->font,
                "press space to start",
                (vec2f_t ){-0.5f, -0.25f},
                0.05f);
    }
    window_gl_render_end(pong->window);

    if (window_keyboard_is_key_just_pressed(pong->window,SDLK_ESCAPE)) game_set_current_state(pong, EXIT);
    if (window_keyboard_is_key_just_pressed(pong->window, SDLK_SPACE))   game_set_current_state(pong, PLAYING);

}

void game_state_player_won(game_pong_t *pong)
{
    switch(pong->current_state)
    {
        case PLAYER01_WON:
            window_gl_render_begin(pong->window); 
            {
                gl_ascii_font_render_text(
                        pong->font,
                        "Player01 Won !",
                        (vec2f_t ){-0.65f, 0.25f},
                        0.10f);
            }
            window_gl_render_end(pong->window);
        break;

        case PLAYER02_WON:
            window_gl_render_begin(pong->window); 
            {
                gl_ascii_font_render_text(
                        pong->font,
                        "Player02 Won !",
                        (vec2f_t ){-0.65f, 0.25f},
                        0.10f);
            }
            window_gl_render_end(pong->window);
        break;
    }

    pong->player01.points = pong->player02.points = 0;

    if (window_keyboard_is_key_just_pressed(pong->window, SDLK_RETURN)) game_set_current_state(pong, MENU);
}

void game_state_exit(game_pong_t *pong)
{
    window_gl_render_begin(pong->window); 
    {
        gl_ascii_font_render_text(
                pong->font,
                "<3",
                (vec2f_t ){-0.5f, 0.2f},
                0.5f);

    }
    window_gl_render_end(pong->window);

    if(window_keyboard_is_key_just_pressed(pong->window, SDLK_RETURN)) pong->window->is_open = false;

}

void game_state_pause(game_pong_t *pong)
{
    window_gl_render_begin(pong->window); 
    {
        gl_ascii_font_render_text(
                pong->font,
                "Pause",
                (vec2f_t ){-0.9f, 0.45f},
                0.35f);
        gl_ascii_font_render_text(
                pong->font,
                "1. continue",
                (vec2f_t ){-0.3f, -0.25f},
                0.04f);
        gl_ascii_font_render_text(
                pong->font,
                "2. quit",
                (vec2f_t ){-0.3f, -0.35f},
                0.04f);
    }
    window_gl_render_end(pong->window);

    if (window_keyboard_is_key_just_pressed(pong->window, SDLK_ESCAPE)) game_set_current_state(pong, PLAYING); 
    if (window_keyboard_is_key_just_pressed(pong->window, SDLK_RETURN)) game_set_current_state(pong, EXIT); 
    
}

