#version 330

in vec2 position;

out float height;
out float scaling_height_factor;
out vec2 uv;

//in vec3 vpoint;
//in vec2 vtexcoord;
uniform mat4 MVP;
uniform float time;

uniform sampler2D texNoise;      // pass the texture also in the vertex shader to compute the height

in vec3 color;
in vec2 vtexcoord;

void main() {
    uv = vtexcoord;     //(position + vec2(1.0, 1.0)) * 0.5;    // don't know why we're keeping that

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    scaling_height_factor = 2.0;
    height = float(texture(texNoise,uv).x) / scaling_height_factor;        // divide by a scaling factor
    vec3 pos_3d = vec3(uv.x, uv.y, -height);        // or position instead of uv

    gl_Position = MVP * vec4(pos_3d, 1.0);
}
