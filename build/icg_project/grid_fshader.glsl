#version 330

in vec2 uv;
in float height;

out vec3 color;

uniform sampler2D tex;
uniform sampler2D tex2;

void main() {
    float window_width = textureSize(tex,0).x;
    float window_height = textureSize(tex,0).y;
     /// TODO: use gl_FragCoord to build a new [_u,_v] coordinate to query the framebuffer
     float _u = gl_FragCoord.x/window_width;
     float _v = gl_FragCoord.y/window_height;

     color = vec3(0.0);
     vec3 color2 = texture(tex2,uv).rgb;

     if(height<.8){
        color += vec3(0,1-(height+.2),0);
     }
     
     color=(color+color2)/2;

}
