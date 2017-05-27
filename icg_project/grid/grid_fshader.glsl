#version 330

in vec2 uv;
in float height;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

in float fog_factor;

out vec3 color;

uniform vec3 Ld;

uniform sampler2D texNoise;
uniform sampler2D tex_grass;
uniform sampler2D tex_rock;
uniform sampler2D tex_snow;
uniform sampler2D tex_sand;
uniform sampler2D tex_ocean;

uniform float water_height;

uniform float offset_x;
uniform float offset_y;
uniform float diffuse_factor;

uniform int fog_enable;
uniform vec3 fog_color;

vec2 offset=vec2(offset_x,offset_y);

void main() {
     float tex_a=.25; // empirical coefficient to determine texture offset
     vec3 color_rock  = texture(tex_rock,(uv+tex_a*offset) * 10).rgb;
     vec3 color_grass = texture(tex_grass,(uv+tex_a*offset) * 25).rgb;
     vec3 color_snow  = texture(tex_snow,(uv+tex_a*offset) * 25).rgb;
     vec3 color_sand  = texture(tex_sand,(uv+tex_a*offset) * 25).rgb;
     vec3 color_ocean = texture(tex_ocean,(uv+tex_a*offset) * 10).rgb;

     float aRock = clamp((1 - 15 * (height- 0.75) * (height - 0.75)),0,1);
     float aGrass= clamp((1 - 5 * (height- 0.4) * (height - 0.4)),0,1);
     float aSnow = clamp((1 - 5 * (height- 1.0f) * (height - 1.0f)),0,1);
     float aSand = clamp((1 - 500 * (height- water_height) * (height - water_height)),0,1);
     float aOcean= clamp((1 - 15 * (height- 0.0) * (height - 0.0)),0,1);
     float sum=aRock+aGrass+aSnow+aSand+aOcean;

     //blend textures
     vec3 color_blended = (aRock*color_rock+ aGrass*color_grass+ aSnow*color_snow+ aSand*color_sand+ aOcean*color_ocean)/sum;

     //custom material diffuse parameter
     vec3 kd = vec3(diffuse_factor);

     vec3 n = normalize(cross(dFdx(vpoint_mv.xyz),dFdy(vpoint_mv.xyz)));

     float cosDiffuse = dot(n,light_dir);

     if (cosDiffuse > 0.0)
     {
        color += kd*Ld*cosDiffuse;
     }

     color=(color+color_blended)*diffuse_factor*2;

     if(fog_enable == 1){
         color = mix(fog_color, color, fog_factor);
     }

}
