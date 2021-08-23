#include "../lib/basic.h"
#include "../lib/math/shapes.h"
#include "../lib/simple/gl_renderer.h"
#include "../lib/simple/window.h"
#include "../lib/simple/font/gl_ascii_font.h"
#include "../lib/game/2D/collision.h"


#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 920


typedef enum game_state_type {

    GST_MENU,
    GST_PAUSE,
    GST_PLAYING,
    GST_PLAYER01_WON,
    GST_PLAYER02_WON,
    GST_EXIT,

    GST_COUNT

} game_state_type;

typedef struct game {

    game_state_type state;

} game ;



typedef struct player_t {

    quadf_t padle;
    vec2f_t position;
    vec2f_t acceleration;

    f32 width;
    f32 height;
     
    int points;

} player_t;

typedef struct ball_t {

    quadf_t ball_quad;
    vec2f_t position;
    vec2f_t acceleration;

    f32 width;
    f32 height;

} ball_t;

//FIXME: Collision doesnt work when the ball is moving super fast, the ball phases throught it.

int main(void)
{
    window_t window = window_init("pong", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_INIT_VIDEO);
    window_set_background(&window, (vec4f_t) {0.0f, 0.0f, 0.0f, 0.0f});

    gl_shader_t shader          = gl_shader_from_file_init("res/pong.vs", "res/pong.fs");
    gl_renderer2d_t renderer    = gl_renderer2d_init(&shader, NULL); 


    const f32 padle_width               = 0.03f;
    const f32 padle_height              = 0.3f;
    const vec2f_t player_acceleration   = {0.0f, 0.03f};
    const vec2f_t ball_acceleration     = {0.008f, 0.001f};
    const f32 ball_dx_multiplier        = 0.004;  
    const f32 ball_dy_multiplier        = 0.00009;
    const quadf_t net_quad              = quadf_init((vec2f_t ){0.0f, 1.0f}, 0.001f, 2.0f);
    const gl_ascii_font_t font          = gl_ascii_font_init("res/charmap-futuristic_black.png",18, 7); 


    player_t player01 = {
        .position = (vec2f_t) {-0.9f, 0.0f},
        .acceleration = player_acceleration,
        .width = padle_width,
        .height = padle_height,
        .points = 0,
    };

    player_t player02 = {
        .position   = (vec2f_t) {0.8f, 0.0f},
        .acceleration = player_acceleration,
        .width = padle_width,
        .height = padle_height,
        .points = 0,
    };

    ball_t ball = {
        .width = 0.03f,
        .height = 0.03f,
        .position =  (vec2f_t) {-0.03f, 0.03f},
        .acceleration = ball_acceleration,
    };



    f64 dt;
    f64 fps;

    window_game_loop(&window)
    {
        dt = window_get_dt(&window);
        fps = window_get_fps(&window);


        ball.position   = vec2f_translate(ball.position, ball.acceleration);
        player01.padle  = quadf_init(player01.position, player01.width, player01.height);
        player02.padle  = quadf_init(player02.position, player02.width, player02.height);
        ball.ball_quad  = quadf_init(ball.position, ball.width, ball.height); 


        if (ball.position.cmp[X] >= 1.0f) {

            player01.points++;
            ball.position = vec2f(0.0f);
            ball.acceleration = ball_acceleration;

        } else if ( ball.position.cmp[X] <= -1.0f) {

            player02.points++;
            ball.position = vec2f(0.0f);
            ball.acceleration = ball_acceleration;

        }

        if (ball.position.cmp[Y] >= 1.0f 
            || ball.position.cmp[Y] <= -1.0f)
        {
            ball.acceleration.cmp[Y] *= -1;
        }

        if(window.keyboard_handler.is_active) {

            SDL_Keycode key = window.keyboard_handler.key;

            if (key == SDLK_w) {

                player01.position = vec2f_translate(
                        player01.position, 
                        player01.acceleration);

                if(player01.position.cmp[Y] >= 1.0f) 
                    player01.position.cmp[Y] = 1.0f;

            } else if (key == SDLK_s) {

                player01.position = vec2f_translate(
                        player01.position, 
                        vec2f_scale(player01.acceleration, -1));

                if(player01.position.cmp[Y] <= -1.0f - (-0.3f)) 
                    player01.position.cmp[Y] = -1.0f - (-0.3f);
            }

            if (key == SDLK_UP) {

                player02.position = vec2f_translate(
                        player02.position, 
                        player02.acceleration);

                if(player02.position.cmp[Y] >= 1.0f) 
                    player02.position.cmp[Y] = 1.0f;

            } else if (key == SDLK_DOWN) {

                player02.position = vec2f_translate(
                        player02.position, 
                        vec2f_scale(player02.acceleration, -1));

                if(player02.position.cmp[Y] <= -1.0f - (-0.3f)) 
                    player02.position.cmp[Y] = -1.0f - (-0.3f);
            }

            if (key ==  SDLK_q) window.is_open = false;

        }

        if (collision_quad_check_by_AABB(player01.padle, ball.ball_quad))
        {
            ball.acceleration.cmp[X] += ball_dx_multiplier;

            ball.acceleration.cmp[Y] += ball_dy_multiplier;
        }

        if (collision_quad_check_by_AABB(ball.ball_quad, player02.padle))
        {
            ball.acceleration.cmp[X] += ball_dx_multiplier;
            ball.acceleration.cmp[X] *= -1;

            ball.acceleration.cmp[Y] += ball_dy_multiplier;
        }


        window_gl_render_begin(&window);
        {
            
            gl_quad_t quads[4] = {

                // positions                                                        // colors          // texture_coord
                ball.ball_quad.vertex[0].cmp[X], ball.ball_quad.vertex[0].cmp[Y], 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
                ball.ball_quad.vertex[1].cmp[X], ball.ball_quad.vertex[1].cmp[Y], 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
                ball.ball_quad.vertex[2].cmp[X], ball.ball_quad.vertex[2].cmp[Y], 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
                ball.ball_quad.vertex[3].cmp[X], ball.ball_quad.vertex[3].cmp[Y], 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,

                player01.padle.vertex[0].cmp[X], player01.padle.vertex[0].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
                player01.padle.vertex[1].cmp[X], player01.padle.vertex[1].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
                player01.padle.vertex[2].cmp[X], player01.padle.vertex[2].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
                player01.padle.vertex[3].cmp[X], player01.padle.vertex[3].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f,

                player02.padle.vertex[0].cmp[X], player02.padle.vertex[0].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
                player02.padle.vertex[1].cmp[X], player02.padle.vertex[1].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
                player02.padle.vertex[2].cmp[X], player02.padle.vertex[2].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
                player02.padle.vertex[3].cmp[X], player02.padle.vertex[3].cmp[Y], 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f,

                net_quad.vertex[0].cmp[X],net_quad.vertex[0].cmp[Y], 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                net_quad.vertex[1].cmp[X],net_quad.vertex[1].cmp[Y], 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                net_quad.vertex[2].cmp[X],net_quad.vertex[2].cmp[Y], 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
                net_quad.vertex[3].cmp[X],net_quad.vertex[3].cmp[Y], 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f

            };

            gl_batch_t batch = {
                .shape_type= BT_QUAD, 
                .shape_count= 4,
                .vertex_buffer= (gl_vertex_t *)quads,
                .vertex_buffer_size= sizeof(quads)
            };

            char string_buffer[32] = {0};

            snprintf(string_buffer, 32, "%i", player01.points);
            gl_ascii_font_render_text(&font, string_buffer, (vec2f_t) {-0.6f, 1.0f}, 0.1f);

            snprintf(string_buffer, 32, "%i", player02.points);
            gl_ascii_font_render_text(&font, string_buffer, (vec2f_t) {0.5f, 1.0f}, 0.1f);

            snprintf(string_buffer, 32, "FPS:%i", (int )fps);
            gl_ascii_font_render_text(&font, string_buffer, (vec2f_t) {0.76f, 1.0f}, 0.03f);

            gl_renderer2d_draw_from_batch(&renderer, &batch); 


        }
        window_gl_render_end(&window);

        /*window_cap_fps(&window);*/
        /*SDL_Delay(1);*/

    }

    gl_ascii_font_destroy(&font);
    gl_render2d_destroy(&renderer);
    window_destroy(&window);
    

    return 0;
}
