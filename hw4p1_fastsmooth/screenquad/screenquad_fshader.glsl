#version 330

in vec2 uv;

layout (location = 0) out vec3 color1;
layout (location = 1) out vec3 color2;

uniform sampler2D tex;
uniform sampler2D tex_1;
uniform float tex_width;
uniform float tex_height;
uniform float std;
uniform vec2 texOffset;
uniform float kernel[128];
uniform int size;
uniform int pass;

void main() {
    // efficient gaussian filtering
    // inspiration source: http://www.stat.wisc.edu/~mchung/teaching/MIA/reading/diffusion.gaussian.kernel.pdf.pdf
    vec3 gauss_color= vec3(0.0);
    float weight_tot = 0.0;
    if (pass==0){
        for (int i = -size; i <= size; i++){
            gauss_color += texture(tex, uv-vec2(i/tex_width,0)).rgb*kernel[i];
            weight_tot += kernel[i];
        }
        color2 =  gauss_color/weight_tot;
    }
    else{
       for (int i = -size; i <= size; i++){
           gauss_color += texture(tex_1, uv-vec2(i/tex_height,0)).rgb*kernel[i];
           weight_tot += kernel[i];
       }
       color1 =  gauss_color/weight_tot;
    }
   //color1=texture(tex,uv).rgb;
   //color2=texture(tex_1,uv).rgb;

}

