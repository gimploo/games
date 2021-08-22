#include "../lib/basic.h"
#include "../lib/math/shapes.h"
#define GL_LOG_ENABLE
#include "../lib/simple/gl_renderer.h"
#include "../lib/simple/window.h"


#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 920

typedef struct player_t {

    quadf_t padle;
    vec2f_t position;
    vec2f_t acceleration;

    f32 width;
    f32 height;

    f32 dy;

} player_t;

typedef struct ball_t {

    quadf_t ball_quad;
    vec2f_t position;
    vec2f_t acceleration;

    f32 width;
    f32 height;

} ball_t;

vec2f_t rand_vec2f(void)
{
    vec2f_t ball;
    ball.cmp[X] = (float )randint(-1, 1);
    ball.cmp[Y] = (float )randint(-1, 1);
    return ball;
}

bool collision_quad_check_by_AABB(quadf_t left, quadf_t right)
{
    // AABB - AABB collision
    //
    bool x_axis_collision = 
        (left.vertex[1].cmp[X] >= right.vertex[0].cmp[X]) 
        && (right.vertex[1].cmp[X] >= left.vertex[0].cmp[X]);

    printf(QUAD_FMT, QUAD_ARG(left));
    printf(QUAD_FMT, QUAD_ARG(right));

    bool y_axis_collision = 
        (left.vertex[0].cmp[Y] >= right.vertex[3].cmp[Y]) 
        && (right.vertex[0].cmp[Y] >= left.vertex[3].cmp[Y]);

    return x_axis_collision && y_axis_collision;
}


int main(void)
{
    window_t window = window_init(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_INIT_VIDEO);
    window_set_background(&window, (vec4f_t) {0.0f, 0.0f, 0.0f, 0.0f});

    gl_shader_t shader = gl_shader_from_file_init("res/pong.vs", "res/pong.fs");
    gl_renderer2d_t renderer = gl_renderer2d_init(&shader, NULL);

    const f32 padle_width = 0.1f;
    const f32 padle_height = 0.3f;
    const vec2f_t player_acceleration = {0.0f, 0.01f};
    const f32 delta_x = 0.0003;  
    const f32 delta_y = 0.00009;
    const f32 dy_mulipler = -0.0003;

    player_t player01 = {
        .position = (vec2f_t) {-0.9f, 0.0f},
        .acceleration = player_acceleration,
        .width = padle_width,
        .height = padle_height,
        .dy = 1.0f
    };

    player_t player02 = {
        .position   = (vec2f_t) {0.8f, 0.0f},
        .acceleration = player_acceleration,
        .width = padle_width,
        .height = padle_height,
        .dy = 1.0f
    };

    ball_t ball = {
        .width = 0.03f,
        .height = 0.03f,
        .position =  (vec2f_t) {-0.03f, 0.03f},
        .acceleration.cmp[X] = -0.008f,
        .acceleration.cmp[Y] = 0.001f,
    };

    while (window.is_open)
    {
        ball.position = vec2f_translate(ball.position, ball.acceleration);
        player01.padle = quadf_init(player01.position, 0.1f, 0.3f);
        player02.padle = quadf_init(player02.position, 0.1f, 0.3f);
        ball.ball_quad    = quadf_init(ball.position, ball.width, ball.height); 


        if (ball.position.cmp[X] >= 1.0f 
            || ball.position.cmp[X] <= -1.0f )
        {
            ball.position.cmp[X] = 0.0f;
            ball.position.cmp[Y] = 0.0f;
            ball.acceleration.cmp[X] = -0.008f;
            ball.acceleration.cmp[Y] = 0.001f;

        }
        if (ball.position.cmp[Y] >= 1.0f 
            || ball.position.cmp[Y] <= -1.0f)
        {
            ball.acceleration.cmp[Y] *= -1;
        }

        if(window.keyboard_handler.is_active) {

            switch(window.keyboard_handler.key) {

                case SDLK_w:

                    player01.dy = dy_mulipler;  
                    player01.position = vec2f_translate(
                            player01.position, 
                            player01.acceleration);

                    if(player01.position.cmp[Y] >= 1.0f) 
                        player01.position.cmp[Y] = 1.0f;

                    break;

                case SDLK_s:

                    player01.dy = dy_mulipler;  
                    player01.position = vec2f_translate(
                            player01.position, 
                            vec2f_scale(player01.acceleration, -1));

                    if(player01.position.cmp[Y] <= -1.0f - (-0.3f)) 
                        player01.position.cmp[Y] = -1.0f - (-0.3f);

                    break;

                case SDLK_UP:

                    player02.dy = dy_mulipler;  
                    player02.position = vec2f_translate(
                            player02.position, 
                            player02.acceleration);

                    if(player02.position.cmp[Y] >= 1.0f) 
                        player02.position.cmp[Y] = 1.0f;

                    break;

                case SDLK_DOWN:

                    player02.dy = dy_mulipler;  
                    player02.position = vec2f_translate(
                            player02.position, 
                            vec2f_scale(player02.acceleration, -1));

                    if(player02.position.cmp[Y] <= -1.0f - (-0.3f)) 
                        player02.position.cmp[Y] = -1.0f - (-0.3f);

                    break;

                case SDLK_q:
                    window.is_open = false;
                    break;
            }

        } else {

            switch(window.keyboard_handler.key) {
                case SDLK_w:
                    player01.dy = 1.0f;  
                    break;
                case SDLK_s:
                    player01.dy = 1.0f;  
                    break;
                case SDLK_UP:
                    player02.dy = 1.0f;  
                    break;
                case SDLK_DOWN:
                    player02.dy = 1.0f;  
                    break;
                case SDLK_q:
                    window.is_open = false;
                    break;
            }
        }

        if (collision_quad_check_by_AABB(player01.padle, ball.ball_quad))
        {
            ball.acceleration.cmp[X] += delta_x;
            ball.acceleration.cmp[X] *= -1;
            ball.acceleration.cmp[Y] += delta_y * player01.dy;
        }

        if (collision_quad_check_by_AABB(ball.ball_quad, player02.padle))
        {
            ball.acceleration.cmp[X] += delta_x;
            ball.acceleration.cmp[X] *= -1;
            ball.acceleration.cmp[Y] += delta_y * player02.dy;
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


            };

            /*gl_renderer2d_draw_quad(&renderer, quads[2]);*/
            /*gl_renderer2d_draw_quad(&renderer, quads[1]);*/
            /*gl_renderer2d_draw_quad(&renderer, quads[0]);*/

            gl_batch_t batch = {
                .shape_type= BT_QUAD, 
                .shape_count= 3,
                .vertex_buffer= quads,
                .vertex_buffer_size= sizeof(quads)
            };

            gl_renderer2d_draw_from_batch(&renderer, &batch); 

        }
        window_gl_render_end(&window);

        // TODO: add delta time for consistent frame rate 
        SDL_Delay(1/60000);
    }

    gl_render2d_destroy(&renderer);
    window_destroy(&window);
    

    return 0;
}
