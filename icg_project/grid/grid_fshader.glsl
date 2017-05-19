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
uniform sampler2D tex_snow;

uniform float offset_x;
uniform float offset_y;
uniform float diffuse_factor;

vec2 offset=vec2(offset_x,offset_y);

void main() {
     float tex_a=.25; // empirical coefficient to determine texture offset
     vec3 color_rock  = texture(tex_rock,(uv+tex_a*offset) * 10).rgb;
     vec3 color_grass = texture(tex_grass,(uv+tex_a*offset) * 25).rgb;
     vec3 color_snow  = texture(tex_snow,(uv+tex_a*offset) * 25).rgb;

     float aRock = clamp((1 - 15 * (height- 0.75) * (height - 0.75)),0,1);
     float aGrass= clamp((1 - 5 * (height- 0.4) * (height - 0.4)),0,1);
     float aSnow = clamp((1 - 5 * (height- 1.0f) * (height - 1.0f)),0,1);
     float sum=aRock+aGrass+aSnow;

     //blend textures
     vec3 color_blended = (aRock*color_rock+aGrass*color_grass+aSnow*color_snow)/sum;

     //custom material diffuse parameter
     vec3 kd = vec3(diffuse_factor);

     vec3 n = normalize(cross(dFdx(vpoint_mv.xyz),dFdy(vpoint_mv.xyz)));

     float cosDiffuse = dot(n,light_dir);

     if (cosDiffuse > 0.0)
     {
        color += kd*Ld*cosDiffuse;
     }
     color=(color+color_blended)*diffuse_factor*2;

}
