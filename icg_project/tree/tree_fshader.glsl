#version 330

#define M_PI 3.14159265358979323846
#define N_RAND 512

in vec2 uv;

out vec4 color;

uniform sampler2D tex;
uniform float lacunarityUni;
uniform int octavesUni;
uniform float frequencyUni;
uniform float amplitudeUni;
uniform float hUni;
uniform float Perm[N_RAND];
uniform float offset_x;
uniform float offset_y;
uniform float persistance;

int window_width = 800;
int window_height = 600;

float gain = 2.0;

vec2 offset = vec2(offset_x,offset_y);

float rand(vec2 co)
{
    return (fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453));
}



void main() {
    color = vec4(texture(tex, uv).rgba);
}


