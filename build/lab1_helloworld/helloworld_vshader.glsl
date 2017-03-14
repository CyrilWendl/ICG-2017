#version 330

in vec3 vpoint;
out vec3 COLORS[3];

void main() {
    gl_Position = vec4(vpoint, 1.0);
    COLORS[0] = vec3(1.0,0.0,0.0);
    COLORS[1] = vec3(0.0,1.0,0.0);
    COLORS[2] = vec3(0.0,0.0,1.0);
    glVertexID(vshader built-in);
}
