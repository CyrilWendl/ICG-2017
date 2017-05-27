#version 330

#define M_PI 3.14159265358979323846
#define N_RAND 512

in vec2 uv;
in float dist;

in float fog_factor;

out vec4 color;

uniform sampler2D tex;
uniform float lacunarityUni;
uniform int octavesUni;
uniform float frequencyUni;
uniform float amplitudeUni;
uniform float hUni;
uniform float offset_x;
uniform float offset_y;
uniform float persistance;
uniform int fog_enable;
uniform vec3 fog_color;

int window_width = 800;
int window_height = 600;
vec2 offset = vec2(offset_x,offset_y);

float rand(vec2 co)
{
    return (fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453));
}



void main() {
    color = vec4(texture(tex, uv).rgba);
    // get rid of the black background (blackground haha) part of the texture
    vec4 fog = vec4(0.8, 0.8, 0.8, color.a);
    color = mix(color, fog, clamp(dist * dist / (9 * 9), 0, 1));

    if(fog_enable == 1){
        color = mix(vec4(fog_color, 1.0), color, fog_factor);
    }

    if (color.a < 0.5) {
        discard;
    }
}


