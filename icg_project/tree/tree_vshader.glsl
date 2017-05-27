#version 330

in vec2 position;

out vec2 uv;
out float height;       // out the height to know if tree should be snowy for example

out float fog_factor;

uniform mat4 view;
uniform mat4 MVP;

uniform float time;
uniform float tree_height;
uniform float tree_size;
uniform float offset_x;
uniform float offset_y;
uniform float treePos_x;
uniform float treePos_y;
uniform sampler2D texNoise;     // pass the terrain to compute base of the tree
out float dist;     // distance the tree (to compute fog)

uniform float fog_density;
const float gradient = 1.5f;

void main() {
    vec2 offset=vec2(offset_x,offset_y);

    float AWindx = 0.005;      // amplitude of the wind on x
    float AWindy = 0.01;      // amplitude of the wind on y
    float freqx = 0.2;      // frequency of the wind on x
    float freqy = 0.3;      // frequency of the wind on y

    vec2 pos = position + vec2(treePos_x,treePos_y);
    pos +=8.*vec2(-offset_x,-offset_y);     // to keep the tree in place

    // put the texture according to the tree size and height
    uv.x = (position.x + tree_size) * (1/tree_size) / 2.0;
    uv.y = (position.y + tree_height) * (1/tree_height) / 2.0;

    vec2 posTerrain = (pos + vec2(16.0)) / 32.0f;       // gives the corresponding position on the terrain

    height = float(texture(texNoise,posTerrain).x);
    vec3 pos_3d = vec3(pos.x+AWindx*sin(freqx*time) ,
                       40.*(position.y+tree_height)+height,     // put the tree straight
                       pos.y+AWindy*sin(freqy*time));

    // Setting up fog
    vec4 pos_to_cam = view * vec4(pos_3d, 1.0);
    float range = length(pos_to_cam.xyz);
    fog_factor = exp(-pow(range * fog_density, gradient));
    fog_factor = clamp(fog_factor, 0.0, 1.0);

    gl_Position = MVP * vec4(pos_3d, 1.0);
    dist = length(gl_Position.xyz);
}
