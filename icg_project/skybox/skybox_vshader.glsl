#version 330 core
layout (location = 0) in vec3 sky_position;
out vec3 TexCoords;

uniform mat4 MVP;

void main()
{
    gl_Position =   MVP *  vec4(sky_position, 1.0);
    TexCoords = sky_position;
}
