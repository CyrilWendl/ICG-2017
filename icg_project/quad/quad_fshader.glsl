#version 330

#define M_PI 3.14159265358979323846
#define NUM_OCTAVES 6

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform float HUni;
uniform float lacunarityUni;
uniform int octavesUni;

float rand(vec2 co){return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);}
float rand (vec2 co, float l) {return rand(vec2(rand(co), l));}
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}


// p must be normalized!
float perlin(vec2 p) {      // perlin noise function

    float dim = 25.0;           // parameter to change frequency
    vec2 pos = floor(p * dim);
    vec2 posx = pos + vec2(1.0, 0.0);
    vec2 posy = pos + vec2(0.0, 1.0);
    vec2 posxy = pos + vec2(1.0);

    // For exclusively black/white noise
    float c = step(rand(pos, dim), 0.5);
    float cx = step(rand(posx, dim), 0.5);
    float cy = step(rand(posy, dim), 0.5);
    float cxy = step(rand(posxy, dim), 0.5);

    /*float c = rand(pos, dim);
    float cx = rand(posx, dim);
    float cy = rand(posy, dim);
    float cxy = rand(posxy, dim);*/

    vec2 d = fract(p * dim);
    d = -0.5 * cos(d * M_PI) + 0.5;

    float ccx = mix(c, cx, d.x);
    float cycxy = mix(cy, cxy, d.x);
    float center = mix(ccx, cycxy, d.y);

    return center * 2.0 - 1.0;
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

float fbmB (in vec2 st) {
    // Initial values
    float value = 0.0;
    float amplitude = 0.6;
    float frequency = 3;
    int octaves = 8;
    //
    // Loop of octaves
    for (int i = 0; i < octaves; i++) {
        value += amplitude * noise(st);
        st *= frequency;
        amplitude *= 0.5;       // change amplitude change here
    }
    return value;
}

float fbm(vec2 x) {         // fractional Brownian motion function
    float v = 0.5;      // parameters to vary
    float a = 0.25;     // parameters to vary
    vec2 shift = vec2(100);     // parameters to vary
    // Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));      // parameters to vary
    for (int i = 0; i < NUM_OCTAVES; ++i) {
            v += a * perlin(x);
            x = rot * x * 2.0 + shift;
            a *= 0.5;
    }
    return v;
}

float fbmClass(vec2 point)
{
    float H= 0.9;
    float lacunarity=10;
    int octaves=4;
    float value = 0.0;
    /* inner loop of fractal construction */
    for (int i = 0; i < octaves; i++) {
        value += perlin(point) * pow(lacunarity, -H*i);
        point *= lacunarity;
    }
    return value;

}


void main() {
   //color = vec3(fbmClass(uv),fbmClass(uv),fbmClass(uv));
   // color = vec3(fbm(uv),fbm(uv),fbm(uv));
    color = vec3(fbmB(uv),fbmB(uv),fbmB(uv));


}


