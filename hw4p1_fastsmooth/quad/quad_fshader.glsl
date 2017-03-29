#version 330

in vec2 uv;

layout (location = 0) out vec3 color;
layout (location = 1) out vec3 color2;

uniform sampler2D tex;

void main() {
    color = texture(tex,uv).rgb;
    color2 = texture(tex,uv).rgb;
}
