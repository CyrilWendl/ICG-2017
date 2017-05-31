#version 330 core
in vec4 particle_position;
out vec2 TexCoords;

uniform mat4 MVP;

void main()
{
    gl_Position =   MVP *  particle_position;
    TexCoords = particle_position.zw;
}
