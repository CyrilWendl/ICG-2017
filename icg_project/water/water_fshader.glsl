#version 330

in vec2 uv;
in float height;
in float scaling_height_factor;

in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

out vec4 color;

uniform vec3 Ld;
uniform float diffuse_factor;

uniform sampler2D texWater;
uniform sampler2D tex_mirror;
uniform sampler2D tex_refract;

void main() {
    float window_width = textureSize(tex_refract,0).x;
    float window_height = textureSize(tex_refract,0).y;
     /// use gl_FragCoord to build a new [_u,_v] coordinate to query the framebuffer
    // _u,_v give direction always normal to the camera
    float _u = gl_FragCoord.x/window_width;
    float _v_refract = gl_FragCoord.y/window_height;
    float _v_reflect = 1.0 - gl_FragCoord.y/window_height;

     //color = mix(texture(texWater,uv ).rgb, vec3(0.13,0.25,0.5), 0.9);

    vec4 color_refract = texture(tex_refract,vec2(_u, _v_refract));
    vec4 color_reflect  = texture(tex_mirror,vec2(_u, _v_reflect));
    vec4 color_water = vec4(64.0f/255.0f, 164.0f/255.0f, 223.0f/255.0f, 0.2f);
    vec4 color_water2 = vec4(0.13,0.25,0.5, 0.5);
    color = mix(color_refract, color_reflect, 0.5);
    color = mix(color, color_water2, 0.5);

    //color = mix(texture(texWater,uv ).rgb, texture(tex_mirror,vec2(_u,_v)).rgb, 3.0);
    //color = mix(color, vec3(0.13,0.25,0.5), 0.5);
    //color = mix(vec3(0.13,0.25,0.5), texture(tex_mirror,vec2(_u,_v)).rgb, 0.2);

     // custom material diffuse parameter
     vec3 kd = vec3(diffuse_factor);
     vec3 n = normalize(cross(dFdx(vpoint_mv.xyz), dFdy(vpoint_mv.xyz)));

     float cosDiffuse = dot(n,light_dir);

     if (cosDiffuse > 0.0)
     {
        color += vec4(kd, 1.0f) *vec4(Ld, 1.0f) *cosDiffuse;
     }


     color = color*(diffuse_factor+.5);


}
