#version 330 core
layout (location = 0) in vec4 particle_position; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform vec2 offset;
uniform vec4 color;

void main()
{
    float scale = 10.0f;
    TexCoords = particle_position.zw;
    ParticleColor = color;
    gl_Position = projection * vec4((particle_position.xy * scale) + offset, 0.0, 1.0);
}
