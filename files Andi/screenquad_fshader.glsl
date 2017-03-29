#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform sampler2D temp_tex;

uniform float tex_width;
uniform float tex_height;

uniform int pass;
uniform int radius;
uniform int circumference;

uniform float kernel[512];

layout (location = 1) out vec3 temp;

float rgb_2_luma(vec3 c) {
    return 0.3*c[0] + 0.59*c[1] + 0.11*c[2];
}

void main() {
     vec3 gauss_color = vec3(0.0);
     float tot_weight = 0.0f;
     if (pass == 0) {
         for(int i=-radius; i <= radius; i++) {
             gauss_color += texture(tex, uv-vec2(i/tex_width,0)).rgb * kernel[i+radius];
             tot_weight += kernel[i+radius];
         }
         temp = gauss_color / tot_weight;
      } else {
          for (int i = -radius; i <= radius; i++) {
             gauss_color += texture(temp_tex, uv-vec2(0,i/tex_height)).rgb * kernel[i+radius];
             tot_weight += kernel[i+radius];
           }
          color = gauss_color / tot_weight;
      }
      //color = gauss_color / tot_weight;
}
