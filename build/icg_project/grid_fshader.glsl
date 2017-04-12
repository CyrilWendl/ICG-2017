#version 330

in vec2 uv;
in float height;

in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

uniform vec3 Ld;

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
     //custom material diffuse parameter
     vec3 kd = vec3(0.6);
     vec3 n = normalize(cross(dFdx(vpoint_mv.xyz),dFdy(vpoint_mv.xyz)));

     float cosDiffuse = dot(n,light_dir);

     if (cosDiffuse > 0.0)
     {
         color += kd*Ld*cosDiffuse;
     }

     color =(color+color2)/2;

}
