#version 330

in vec2 position;

out float height;
out vec2 uv;

out vec4 vpoint_mv;
out vec3 light_dir, view_dir;

//in vec3 vpoint;
//in vec2 vtexcoord;
uniform mat4 MVP;
uniform float time;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

uniform sampler2D tex;      // pass the texture also in the vertex shader to compute the height

in vec3 color;

void main() {
    mat4 MV = view * model;
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    float scaling_height_factor = 1.0;
    height = float(texture(tex,uv).x) / scaling_height_factor;        // divide by a scaling factor
    vec3 pos_3d = vec3(position.x, -height, -position.y);

    vpoint_mv = MV * vec4(pos_3d, 1.0);
    gl_Position = MVP * vec4(pos_3d, 1.0);

    light_dir = light_pos - vpoint_mv.xyz;
    light_dir = normalize(light_dir);

    view_dir = -vpoint_mv.xyz;
    view_dir = normalize(view_dir);
}
