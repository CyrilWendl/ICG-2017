#version 330 core
in vec2 uv;
uniform sampler2D colorTex;
uniform sampler2D velocityTex;
out vec4 color;

void main() {
    /// TODO: use the velocity vector stored in velocityTex to compute the line integral
    /// TODO: use a constant number of samples for integral (what happens if you take too few?)
    /// HINT: you can scale the velocity vector to make the motion blur effect more prominent
    /// HINT: to debug integration don't use the velocityTex, simply assume velocity is constant, e.g. vec2(1,0)
    //color = texture(colorTex, uv);
    vec2 velocity = texture(velocityTex, uv).xy;
    int N = 30;
    vec4 color_total = vec4(0.0, 0.0, 0.0, 0.0);
    for(int i = 0; i < N; i++) {
        //proper indexing i/N since we need values from 0 to 1
        color_total += texture(colorTex, ((uv + velocity*(float(i)/float(N)))));
    }
    color = color_total / float(N);
}
