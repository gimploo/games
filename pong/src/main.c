#define WINDOW_SDL
#include <poglib/application.h>
#include <poglib/font/glbitmapfont.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 920

typedef struct player_t {

    quadf_t padle;
    vec3f_t position;
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
    glrenderer2d_t *renderer;
    const glbitmapfont_t *font;

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
    window_t * window = window_init("pong", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_INIT_VIDEO);
    window_set_background(window, (vec4f_t){0.0f, 0.0f, 0.0f, 0.0f});

    glshader_t shader          = glshader_from_file_init("res/pong.vs", "res/pong.fs");
    glrenderer2d_t renderer    = glrenderer2d(&shader, NULL); 

    const f32 padle_width               = 0.03f;
    const f32 padle_height              = 0.3f;
    const vec2f_t player_speed   = {0.0f, 0.03f};
    const vec2f_t ball_speed     = {0.008f, 0.001f};
    glbitmapfont_t font          = glbitmapfont_init("res/charmap-futuristic_black.png",18, 7); 


    game_pong_t pong = {

        .prev_state     = MENU,
        .current_state  = MENU,

        .player01 = {
            .position = (vec3f_t) {-0.9f, 0.0f, 1.0f},
            .speed = player_speed,
            .width = padle_width,
            .height = padle_height,
            .points = 0,
        },

        .player02 = {
            .position   = (vec3f_t) {0.8f, 0.0f, 1.0f},
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

        .window = window,
        .renderer = &renderer,
        .font = &font

    };


    f64 dt;

    while(window->is_open)
    {
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
                pong.ball.speed.x *= -1;
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

    glbitmapfont_destroy(&font);
    window_destroy();
    return 0;
}

void game_state_playing(game_pong_t *pong)
{
        ball_t ball                     = pong->ball;
        player_t player01               = pong->player01;
        player_t player02               = pong->player02;
        window_t *window                = pong->window;
        glrenderer2d_t *renderer       = pong->renderer;
        const glbitmapfont_t *font     = pong->font;
        const quadf_t net_quad          = quadf((vec3f_t ){0.0f, 1.0f, 0.0f}, 0.001f, 2.0f);
        const f32 ball_dx_multiplier    = 0.004;  
        const f32 ball_dy_multiplier    = 0.0001;

        f64 fps;

        while(window->is_open)
        {   
            ball.position   = vec2f_translate(ball.position, ball.speed);
            player01.padle  = quadf(player01.position, player01.width, player01.height);
            player02.padle  = quadf(player02.position, player02.width, player02.height);
            ball.ball_quad  = quadf(ball.position, ball.width, ball.height); 


            if (ball.position.x >= 1.0f) {

                player01.points++;
                game_set_current_state(pong, PLAYER01_SCORED);

            } else if ( ball.position.x <= -1.0f) {

                player02.points++;
                game_set_current_state(pong, PLAYER02_SCORED);

            }

            if (ball.position.y >= 1.0f 
                || ball.position.y <= -1.0f)
            {
                ball.speed.y *= -1;
            }

            if (window_keyboard_is_key_just_pressed(window, SDLK_w)) {

                player01.position = vec2f_translate(
                        player01.position, 
                        player01.speed);

                if(player01.position.y >= 1.0f) 
                    player01.position.y = 1.0f;

            } else if (window_keyboard_is_key_just_pressed(window, SDLK_s)) {

                player01.position = vec2f_translate(
                        player01.position, 
                        vec2f_scale(player01.speed, -1));

                if(player01.position.y <= -1.0f - (-0.3f)) 
                    player01.position.y = -1.0f - (-0.3f);
            }

            if (window_keyboard_is_key_just_pressed(window, SDLK_UP)) {

                player02.position = vec2f_translate(
                        player02.position, 
                        player02.speed);

                if(player02.position.y >= 1.0f) 
                    player02.position.y = 1.0f;

            } else if (window_keyboard_is_key_just_pressed(window, SDLK_DOWN)) {

                player02.position = vec2f_translate(
                        player02.position, 
                        vec2f_scale(player02.speed, -1));

                if(player02.position.y <= -1.0f - (-0.3f)) 
                    player02.position.y = -1.0f - (-0.3f);
            }

            if (window_keyboard_is_key_just_pressed(window, SDLK_ESCAPE)) {
                game_set_current_state(pong, PAUSE);
            }


            if (collision_quad_check_by_AABB(player01.padle, ball.ball_quad))
            {
                ball.speed.x -= ball_dx_multiplier;
                ball.speed.x *= -1;

                ball.speed.y += ball_dy_multiplier;
            }

            if (collision_quad_check_by_AABB(ball.ball_quad, player02.padle))
            {
                ball.speed.x += ball_dx_multiplier;
                ball.speed.x *= -1;

                ball.speed.y += ball_dy_multiplier;
            }


            window_glrender_begin(window);
            {
                
                glquad_t quads[4] = {

                    // positions                                                            // colors          // texture_coord
                    ball.ball_quad.vertex[0].x, ball.ball_quad.vertex[0].y, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    ball.ball_quad.vertex[1].x, ball.ball_quad.vertex[1].y, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    ball.ball_quad.vertex[2].x, ball.ball_quad.vertex[2].y, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    ball.ball_quad.vertex[3].x, ball.ball_quad.vertex[3].y, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,

                    player01.padle.vertex[0].x, player01.padle.vertex[0].y, 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    player01.padle.vertex[1].x, player01.padle.vertex[1].y, 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    player01.padle.vertex[2].x, player01.padle.vertex[2].y, 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    player01.padle.vertex[3].x, player01.padle.vertex[3].y, 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,

                    player02.padle.vertex[0].x, player02.padle.vertex[0].y, 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    player02.padle.vertex[1].x, player02.padle.vertex[1].y, 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    player02.padle.vertex[2].x, player02.padle.vertex[2].y, 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,
                    player02.padle.vertex[3].x, player02.padle.vertex[3].y, 0.0f,   1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f,

                    net_quad.vertex[0].x,net_quad.vertex[0].y, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                    net_quad.vertex[1].x,net_quad.vertex[1].y, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                    net_quad.vertex[2].x,net_quad.vertex[2].y, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                    net_quad.vertex[3].x,net_quad.vertex[3].y, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f

                };

                glbatch_t batch = {
                    .shape_type= BT_QUAD, 
                    .shape_count= 4,
                    .vertex_buffer= (glvertex_t *)quads,
                    .vertex_buffer_size= sizeof(quads)
                };

                char string_buffer[32] = {0};

                snprintf(string_buffer, 32, "%i", player01.points);
                glbitmapfont_font_render_text(font, string_buffer, (vec2f_t) {-0.6f, 1.0f}, 0.1f);

                snprintf(string_buffer, 32, "%i", player02.points);
                glbitmapfont_font_render_text(font, string_buffer, (vec2f_t) {0.5f, 1.0f}, 0.1f);

                snprintf(string_buffer, 32, "FPS:%i", (int )fps);
                glbitmapfont_font_render_text(pong->font, string_buffer, (vec2f_t) {0.76f, 1.0f}, 0.03f);

                glrenderer2d_draw_from_batch(renderer, &batch); 

            }
            window_glrender_end(window);
        }
}

void game_state_menu(game_pong_t *pong)
{
    window_glrender_begin(pong->window); 
    {
        glbitmapfont_font_render_text(
                pong->font,
                "pong",
                (vec2f_t ){-0.7f, 0.25f},
                0.35f);
        glbitmapfont_font_render_text(
                pong->font,
                "press space to start",
                (vec2f_t ){-0.5f, -0.25f},
                0.05f);
    }
    window_glrender_end(pong->window);

    if (window_keyboard_is_key_just_pressed(pong->window,SDLK_ESCAPE)) game_set_current_state(pong, EXIT);
    if (window_keyboard_is_key_just_pressed(pong->window, SDLK_SPACE))   game_set_current_state(pong, PLAYING);

}

void game_state_player_won(game_pong_t *pong)
{
    switch(pong->current_state)
    {
        case PLAYER01_WON:
            window_glrender_begin(pong->window); 
            {
                glbitmapfont_font_render_text(
                        pong->font,
                        "Player01 Won !",
                        (vec2f_t ){-0.65f, 0.25f},
                        0.10f);
            }
            window_glrender_end(pong->window);
        break;

        case PLAYER02_WON:
            window_glrender_begin(pong->window); 
            {
                glbitmapfont_font_render_text(
                        pong->font,
                        "Player02 Won !",
                        (vec2f_t ){-0.65f, 0.25f},
                        0.10f);
            }
            window_glrender_end(pong->window);
        break;
    }

    pong->player01.points = pong->player02.points = 0;

    if (window_keyboard_is_key_just_pressed(pong->window, SDLK_RETURN)) game_set_current_state(pong, MENU);
}

void game_state_exit(game_pong_t *pong)
{
    window_glrender_begin(pong->window); 
    {
        glbitmapfont_font_render_text(
                pong->font,
                "<3",
                (vec2f_t ){-0.5f, 0.2f},
                0.5f);

    }
    window_glrender_end(pong->window);

    if(window_keyboard_is_key_just_pressed(pong->window, SDLK_RETURN)) pong->window->is_open = false;

}

void game_state_pause(game_pong_t *pong)
{
    window_glrender_begin(pong->window); 
    {
        glbitmapfont_font_render_text(
                pong->font,
                "Pause",
                (vec2f_t ){-0.9f, 0.45f},
                0.35f);
        glbitmapfont_font_render_text(
                pong->font,
                "1. continue",
                (vec2f_t ){-0.3f, -0.25f},
                0.04f);
        glbitmapfont_font_render_text(
                pong->font,
                "2. quit",
                (vec2f_t ){-0.3f, -0.35f},
                0.04f);
    }
    window_glrender_end(pong->window);

    if (window_keyboard_is_key_just_pressed(pong->window, SDLK_ESCAPE)) game_set_current_state(pong, PLAYING); 
    if (window_keyboard_is_key_just_pressed(pong->window, SDLK_RETURN)) game_set_current_state(pong, EXIT); 
    
}

