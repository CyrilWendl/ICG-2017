#version 330
#define MODE 0

in vec2 uv;

layout (location = 0) out vec3 color1;
layout (location = 1) out vec3 color2;

uniform sampler2D tex_1;
uniform float tex_width;
uniform float tex_height;
uniform float std;
uniform vec2 texOffset;
uniform float kernel[9];


void main() {
#if MODE==1
    // gaussian convolution
    // float std_ = .1; // standard deviation (<.3 disable)
    vec3 color_tot = vec3(0,0,0);
    float weight_tot = 0;
    int SIZE = 1 + 2 * 3 * int(ceil(std));
    for(int i=-SIZE; i<=SIZE; i++){
        for(int j=-SIZE; j<=SIZE; j++){
            float w = exp(-(i*i+j*j)/(2.0*std*std*std*std));
            vec3 neigh_color = texture(tex_1, uv+vec2(i/tex_width,j/tex_height)).rgb;
            color_tot += w * neigh_color;
            weight_tot += w;
        }
    }
    color = color_tot / weight_tot; // ensure \int w = 1
#else
    // efficient gaussian filtering
    vec3 color_tot_1= vec3(0.0);

    for (int i = -4; i <= 4; i++){
            color_tot_1 += texture(tex_1,uv+vec2(i/tex_width,i/tex_height)).rgb*kernel[i + 4];

            color1 = color_tot_1;
    }

    vec3 color_tot_2= vec3(0.0);
    for (int i = -4; i <= 4; i++){
        color_tot_2 += texture(tex_1,uv+vec2(i/tex_width,i/tex_height)).rgb*kernel[i + 4];
        color2 = color_tot_2;
    }

#endif
}

