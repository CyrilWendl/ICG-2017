#version 330

in vec2 position;

in vec3 vpoint;
//in vec2 vtexcoord;

out vec2 uv;
out float height;       // out the height to know if tree should be snowy for example


uniform mat4 MVP;

uniform float time;
uniform float tree_height;
uniform float offset_x;
uniform float offset_y;
uniform sampler2D texNoise;     // pass the terrain to compute base of the tree


void main() {

    uv = (position + vec2(1.0)) / 2.0;
    height = float(texture(texNoise,uv).x);
    vec3 pos_3d = vec3(position.x , height+.4, position.y);
    gl_Position = MVP * vec4(pos_3d, 1.0);
}
