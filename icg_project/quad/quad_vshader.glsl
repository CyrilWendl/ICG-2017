#version 330

in vec3 vpoint;
//in vec2 vtexcoord;

out vec2 uv;

uniform mat4 MVP;


void main() {
    gl_Position = vec4(vpoint, 1.0);        // MVP * was making the mountain change size with the zoom
    uv = vpoint.xy+4.0f/4.0f; // TODO what is the range of uv?
    //xy normally between -1 and 1, now between -2 and 2
}
