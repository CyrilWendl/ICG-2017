#version 330

in vec2 uv;

layout (location = 0) out vec3 color;

uniform sampler2D tex;

void main() {
    color = texture(tex, uv).rgb;
}
