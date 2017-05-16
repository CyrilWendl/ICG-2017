#version 330

in vec2 uv;
in float height;
in float scaling_height_factor;

in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

out vec3 color;

uniform vec3 Ld;
uniform float diffuse_factor;

uniform sampler2D texWater;
uniform sampler2D tex_mirror;

void main() {
    /**float window_width = textureSize(texWater,0).x;
    float window_height = textureSize(texWater,0).y;*/
    float window_width = textureSize(tex_mirror,0).x;
    float window_height = textureSize(tex_mirror,0).y;
     /// use gl_FragCoord to build a new [_u,_v] coordinate to query the framebuffer
    float _u = gl_FragCoord.x/window_width;
    float _v = 1.0 - gl_FragCoord.y/window_height;       // _u,_v give direction always normal to the camera

     //color = mix(texture(texWater,uv ).rgb, vec3(0.13,0.25,0.5), 0.9);

    //color = mix(texture(texWater,uv ).rgb, texture(tex_mirror,vec2(_u,_v)).rgb, 3.0);
    //color = mix(color, vec3(0.13,0.25,0.5), 0.5);
    color = mix(vec3(0.13,0.25,0.5), texture(tex_mirror,vec2(_u,_v)).rgb, 0.2);

    //color = texture(texWater,uv ).rgb;

     //custom material diffuse parameter
     vec3 kd = vec3(diffuse_factor);
     vec3 n = normalize(cross(dFdx(vpoint_mv.xyz),dFdy(vpoint_mv.xyz)));

     float cosDiffuse = dot(n,light_dir);

     if (cosDiffuse > 0.0)
     {
        color += kd*Ld*cosDiffuse;
     }


     color=color;


}
