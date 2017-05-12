#version 330

in vec2 uv;
in float height;

in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

out vec3 color;

uniform vec3 Ld;

uniform sampler2D texNoise;
uniform sampler2D tex_grass;
uniform sampler2D tex_rock;
uniform float offset_x;
uniform float offset_y;

vec2 offset=vec2(offset_x,offset_y);

void main() {
    //float window_width = textureSize(texNoise,0).x;
    //float window_height = textureSize(texNoise,0).y;
    //textureOffset(tex_grass,uv,offset);
     /// TODO: use gl_FragCoord to build a new [_u,_v] coordinate to query the framebuffer
     //float _u = gl_FragCoord.x/window_width+(1-mod(offset_x,1)=;
     //float _v = gl_FragCoord.y/window_height+offset_y;       // _u,_v give direction always normal to the camera

     color = vec3(0.0);
     vec3 color_rock = texture(tex_rock,uv * 10).rgb;
     vec3 color_grass = texture(tex_grass,(uv+0*(offset+ vec2(16.0)) / 32.0f) * 25).rgb;

     float snow = .8; // minimum height where snow begins
     float snow_z;
     float exp=.6;
     float aRock=clamp((1 - 15 * (height- 0.75) * (height - 0.75)),0,1);
     float aGrass=clamp((1 - 5 * (height- 0.4) * (height - 0.4)),0,1);
     float sum=aRock+aGrass;

     //blend textures

     vec3 color_blended = (aRock*color_rock+aGrass*color_grass)/sum;

     // add snow
     if(height>snow){
        snow_z=pow((height-snow),exp)/pow((1-snow),exp); // exponential function
        color_blended += vec3(snow_z,snow_z,snow_z);
     }

     //custom material diffuse parameter
     vec3 kd = vec3(.3);
     vec3 n = normalize(cross(dFdx(vpoint_mv.xyz),dFdy(vpoint_mv.xyz)));

     float cosDiffuse = dot(n,light_dir);

     if (cosDiffuse > 0.0)
     {
        color += kd*Ld*cosDiffuse;
     }
     color=color+color_blended;

}