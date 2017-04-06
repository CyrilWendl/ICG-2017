#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;

void main() {
    float window_width = textureSize(tex,0).x;
    float window_height = textureSize(tex,0).y;
     /// TODO: use gl_FragCoord to build a new [_u,_v] coordinate to query the framebuffer
     float _u = gl_FragCoord.x/window_width;
     float _v = gl_FragCoord.y/window_height;

     color = texture(tex,uv).rgb;
    //color = vec3(0,0,100);//texture(tex, uv).rgb;
}
