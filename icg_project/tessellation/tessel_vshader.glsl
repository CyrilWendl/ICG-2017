#version 430

in vec2 position;
uniform sampler2D texNoise;
uniform mat4 mvp;
uniform mat4 model;
uniform mat4 view;
uniform float time;
uniform vec3 light_pos;

out vec4 vpoint_mv;
out vec3 light_dir, view_dir;

out float height;
out float scaling_height_factor;
out vec2 uv;

out vec3 vposition;

void main(void){
    vec2 texcoord = (position + vec2(1.0)) * 0.5;
    scaling_height_factor = 4.0;
    height = float(texture(texNoise,texcoord).x) / scaling_height_factor;
    vec4 displaced = vec4(position.x, position.y, height, 1.0);

    gl_Position = mvp * displaced;

    mat4 MV = view * model;

    vpoint_mv = MV * displaced;

    light_dir = light_pos - vpoint_mv.xyz;
    light_dir = normalize(light_dir);

    view_dir = -vpoint_mv.xyz;
    view_dir = normalize(view_dir);

}
