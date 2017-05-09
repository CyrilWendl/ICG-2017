#version 430

in vec2 texcoord;
in float depth;
out vec4 fragment;
out vec3 color;

uniform sampler2D texNoise;
uniform sampler2D tex2;
uniform vec3 Ld;

in float teheight;
in vec4 tepoint_mv;
in vec3 telight_dir, teview_dir;
in vec3 teposition;

void main(){

    color = vec3(0.0);
    float z=4.0*teheight; // current height taking into account scaling factor
    float snow = .4; // minimum height where snow begins
    float snow_z;
    float exp=.8;
    if(z>snow){
       snow_z=pow((z-snow),exp)/pow((1-snow),exp); // exponential function
       color += vec3(snow_z,snow_z,snow_z);
    }

    if(z<.3){
       color=vec3(0,0,1);
    }
    if(z>.3&&z<.35){
        snow_z=pow((z-snow),exp)/pow((1-snow),exp); // exponential function
        color=vec3(0,0,1);
    }

    //custom material diffuse parameter
    vec3 kd = vec3(0.3);
    vec3 n = normalize(cross(dFdx(tepoint_mv.xyz),dFdy(tepoint_mv.xyz)));

    float cosDiffuse = dot(n,telight_dir);

    if (cosDiffuse > 0.0)
    {
       color += kd*Ld*cosDiffuse;
    }
    //fragment = mix(color, color*0.5+vec4(0.5, 0.5, 0.5, 1.0), depth*2.0);

}
