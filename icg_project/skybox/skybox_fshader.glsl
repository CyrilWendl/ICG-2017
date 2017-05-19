#version 330 core
in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox_day;
uniform samplerCube skybox_night;
uniform float blend;

void main()
{
    vec4 color1 = texture(skybox_day, TexCoords);
    vec4 color2 = texture(skybox_night, TexCoords);
    color = mix(color1, color2, blend);
}
