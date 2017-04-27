#version 330

in vec2 position;
in vec3 color;
//in vec2 vtexcoord;

out vec4 vpoint_mv;
out vec3 light_dir, view_dir;

out float height;
out float scaling_height_factor;
out vec2 uv;

//in vec3 vpoint;
//in vec2 vtexcoord;
uniform mat4 MVP;
uniform float time;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

uniform sampler2D texNoise;      // pass the texture also in the vertex shader to compute the height



void main() {
    uv = (position + vec2(1.0)) * 0.5;    // don't know why we're keeping that
    mat4 MV = view * model;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    scaling_height_factor = 2.0; // is this still necessary?
    height = float(texture(texNoise,uv).x) / scaling_height_factor;        // divide by a scaling factor
    vec3 pos_3d = vec3(position.x, position.y, height);        // or position instead of uv

    vpoint_mv = MV * vec4(pos_3d, 1.0);
    gl_Position = MVP * vec4(pos_3d, 1.0);

    light_dir = light_pos - vpoint_mv.xyz;
    light_dir = normalize(light_dir);

    view_dir = -vpoint_mv.xyz;
    view_dir = normalize(view_dir);
}
