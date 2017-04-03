#version 330

in vec2 position;

out vec2 uv;

//in vec3 vpoint;
//in vec2 vtexcoord;
uniform mat4 MVP;
uniform float time;

uniform sampler2D tex;      // pass the texture also in the vertex shader to compute the height

in vec3 color;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    float scaling_height_factor = 10;
    float height = float(texture(tex,uv).x) / scaling_height_factor;        // divide by a scaling factor
    vec3 pos_3d = vec3(position.x, height, -position.y);

    gl_Position = MVP * vec4(pos_3d, 1.0);
}
