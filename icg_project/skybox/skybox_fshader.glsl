#version 330 core
in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox_day;
uniform samplerCube skybox_night;

void main()
{
    vec4 color_day = texture(skybox_day, TexCoords);
    vec4 color_night = texture(skybox_night, TexCoords);
    color = mix(color_day, color_night, 0.5f);
}
