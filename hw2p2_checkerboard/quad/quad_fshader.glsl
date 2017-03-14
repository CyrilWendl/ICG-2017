#version 330

in vec2 uv;

out vec3 color;

uniform sampler1D colormap;

const float M_PI = 3.1415926535897932384626433832795f;

void main() {
    float grid_size = 10.0f;
    float scale = grid_size / 2.0f;
    //remapping [0,1] to [0, 2pi], then add 1 to ensure positive values.
    float value_u = (sin(2.0f * M_PI * uv.x * scale));
    float value_v = (sin(2.0f * M_PI * uv.y * scale));
    float value = ((value_u * value_v) + 1.0f) * 0.5f;
    color = texture(colormap, value).rgb;
}


