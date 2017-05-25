#version 330

in vec2 position;
in vec3 color;

out vec4 vpoint_mv;
out vec3 light_dir, view_dir;

out float height;
out vec2 uv;

uniform mat4 MVP;
uniform float time;

uniform float water_height;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;


void main() {
    uv = position+16.0f/32.0f;
    mat4 MV = view * model;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    float fStat = 3.1415/4.0;      // frequency of the stationnary waves
    float fPert = 3.1415;       // frequency of the perturbations (real waves)
    float kStat = 10;           // wave vector for the stationnary waves
    float kPert = 10;            // wave vector for the perturbative waves
    float AStat = 0.0025;        // amplitude for the stationnary waves
    float APert = 0.005;         // amplitude for the perturbative waves
    height = AStat * cos(kStat*position.x)*sin(kStat*position.x) * cos(kStat*position.y)*sin(kStat*position.y) * sin(fStat*time) + APert * sin(kPert*sqrt(position.x*position.x+position.y*position.y) - fPert*time);
    height += water_height;
    vec3 pos_3d = vec3(position.x, height, position.y);

    gl_Position = MVP * vec4(pos_3d, 1.0);;

    vpoint_mv = MV * vec4(pos_3d, 1.0);

    light_dir = light_pos - vpoint_mv.xyz;
    light_dir = normalize(light_dir);

    view_dir = -vpoint_mv.xyz;
    view_dir = normalize(view_dir);
}
