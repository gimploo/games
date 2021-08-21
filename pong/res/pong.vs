#version 330

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_color;

out vec3 color;

void main()
{
    gl_Position = vec4(v_pos, 1.0);
    color = v_color;
}

