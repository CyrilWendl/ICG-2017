#version 330 core
in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox_day;
uniform samplerCube skybox_night;
uniform float blend;

uniform int fog_enable;
uniform vec3 fog_color;

const float fog_start = 0.0f;
const float fog_end = 0.3f;

void main()
{
    vec4 color1 = texture(skybox_day, TexCoords);
    vec4 color2 = texture(skybox_night, TexCoords);
    color = mix(color1, color2, blend);

    if(fog_enable == 1)
    {
       float fog_factor = (TexCoords.y - fog_start) / (fog_end - fog_start);
       fog_factor = clamp(fog_factor, 0.0, 1.0);
       color = mix(vec4(fog_color, 1.0), color, fog_factor);
    }
}
