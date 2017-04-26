#version 330

in vec2 uv;
in float height;
in float scaling_height_factor;

in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

out vec3 color;

uniform vec3 Ld;

uniform sampler2D texNoise;
uniform sampler2D tex2;

void main() {
    float window_width = textureSize(texNoise,0).x;
    float window_height = textureSize(texNoise,0).y;
     /// TODO: use gl_FragCoord to build a new [_u,_v] coordinate to query the framebuffer
     float _u = gl_FragCoord.x/window_width;
     float _v = gl_FragCoord.y/window_height;       // _u,_v give direction always normal to the camera

     color = vec3(0.0);
     vec3 color2 = texture(tex2,uv).rgb;

     float z=scaling_height_factor*height; // current height taking into account scaling factor
     float snow = .4; // minimum height where snow begins
     float snow_z;
     float exp=.8;
     if(z>snow){
        snow_z=pow((z-snow),exp)/pow((1-snow),exp); // exponential function
        color += vec3(snow_z,snow_z,snow_z);
     }

     //custom material diffuse parameter
     vec3 kd = vec3(0.6);
     vec3 n = normalize(cross(dFdx(vpoint_mv.xyz),dFdy(vpoint_mv.xyz)));

     float cosDiffuse = dot(n,light_dir);

     if (cosDiffuse > 0.0)
     {
        color += kd*Ld*cosDiffuse;
     }
     
     color=color+color2;

}
