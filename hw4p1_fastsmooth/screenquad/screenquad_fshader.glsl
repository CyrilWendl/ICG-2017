#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform sampler2D tex_1;

uniform float tex_width;
uniform float tex_height;

uniform float std;
uniform int pass;

void main() {
    int size=6*int(ceil(std));
    vec3 gauss_color=vec3(0.0);
    float tot_weight;
    float gauss;
    if (pass==0){
         for(int i=-size;i<=size;i++){
            gauss=exp(-(i*i)/(2*std*std*std*std));
            gauss_color += texture(tex, uv+vec2(i/tex_width,0)).rgb * gauss;
            tot_weight += gauss;
         }
        color=gauss_color/tot_weight;
    } else{
    for(int i=-size;i<=size;i++){
        gauss=exp(-(i*i)/(2*std*std*std*std));
        gauss_color += texture(tex, uv+vec2(0,i/tex_height)).rgb * gauss;
        tot_weight += gauss;
     }
        color=gauss_color/tot_weight;
    }
}

