#version 330

in vec3 COLORS[3];
out vec3 color[3];

void main() {
    color = COLORS;
}
