#version 330

in vec2 position;

out vec2 uv;

//in vec3 vpoint;
//in vec2 vtexcoord;
uniform mat4 MVP;
uniform float time;

in vec3 color;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    // Animate the height of the grid points as a sine function of the
    // 'time' and the position ('uv') within the grid.
    float height = color.x * 100;//0.05*cos(20*uv.x)*sin(10*(uv.x))*cos(10*uv.y)*sin(5*(uv.y))*sin(3.1415*time) + 0.05*sin(20*(uv.x*uv.x+uv.y*uv.y) - 2*3.14*time);
    vec3 pos_3d = vec3(position.x, height, -position.y);

    gl_Position = MVP * vec4(pos_3d, 1.0);
}
