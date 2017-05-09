#version 430
// http://codeflow.org/entries/2010/nov/07/opengl-4-tessellation/
layout(triangles, equal_spacing, cw) in;
out vec2 texcoord;
out float depth;

in float tcheight;
out float teheight;
in vec4 tcpoint_mv;
out vec4 tepoint_mv;

in vec3 tclight_dir, tcview_dir;
out vec3 telight_dir, teview_dir;

in vec3 tcposition[];
out vec3 teposition;

uniform sampler2D texNoise;
uniform mat4 mvp;

void main(){
    tepoint_mv = tcpoint_mv;
    teheight = tcheight;
    telight_dir = tclight_dir;
    teview_dir = tcview_dir;
//    float u = gl_TessCoord.x;
//    float v = gl_TessCoord.y;

//    vec4 a = mix(gl_in[1].gl_Position, gl_in[0].gl_Position, u);
//    vec4 b = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, u);
//    vec4 position = mix(a, b, v);
//    texcoord = position.xy;
////   float height = texture(texNoise, texcoord).a;
//    vec3 p0 = gl_TessCoord.x * tcposition[0];
//    vec3 p1 = gl_TessCoord.y * tcposition[1];
//    vec3 p2 = gl_TessCoord.z * tcposition[2];

//    teposition = normalize (p0+p1+p2);
//    gl_Position = mvp * vec4(texcoord.xy, teheight, 1.0);
//    depth = gl_Position.z;
}
