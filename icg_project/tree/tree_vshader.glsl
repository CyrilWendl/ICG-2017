#version 330

in vec2 position;

out vec2 uv;
out float height;       // out the height to know if tree should be snowy for example


uniform mat4 MVP;

uniform float time;
uniform float tree_height;
uniform float offset_x;
uniform float offset_y;
uniform sampler2D texNoise;     // pass the terrain to compute base of the tree


void main() {
    vec2 offset=vec2(offset_x,offset_y);

    float AWindx = 0.05;      // amplitude of the wind on x
    float AWindy = 0.1;      // amplitude of the wind on y

    vec2 pos = position;
    pos +=8.*vec2(-offset_x,-offset_y);     // to keep the tree in place

    uv = (position + vec2(1.0)) * (1 / tree_height) / 2.0;
    uv.x += 0.5;
    uv.y += 0.5;

    vec2 posTerrain = (pos + vec2(16.0)) / 32.0f;

    height = float(texture(texNoise,posTerrain).x);
    vec3 pos_3d = vec3(pos.x/**AWindx*sin((position.z)*time)*/ , /*put the tree straight*/ 10.*position.y+tree_height+height, pos.y/**AWindy*sin((position.z)*time)*/);
    //vec3 pos_3d = vec3(pos.x , position.z+height, pos.y);
    gl_Position = MVP * vec4(pos_3d, 1.0);
}
