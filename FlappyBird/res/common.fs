#version 330 core
in vec4 color;
in vec2 tex_coord;

uniform sampler2D texture01;

out vec4 FragColor;

void main()
{
    vec4 tex = texture(texture01, tex_coord);
    if (tex.a < 0.1)
        discard;
    FragColor = tex;
}
