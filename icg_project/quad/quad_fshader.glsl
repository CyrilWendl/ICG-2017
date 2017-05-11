#version 330

#define M_PI 3.14159265358979323846
#define N_RAND 512

in vec2 uv;

out vec3 terrain;

uniform sampler2D tex;
uniform float lacunarityUni;
uniform int octavesUni;
uniform float frequencyUni;
uniform float amplitudeUni;
uniform float hUni;
uniform float Perm[N_RAND];
uniform float offset_x;
uniform float offset_y;

int window_width = 800;
int window_height = 600;

float rand(vec2 co)
{
    return (fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453));
}

vec2 getRandomGradient(vec2 p)
{
    float r = 45 * rand(p);
    return vec2(cos(r), sin(r));
}

float interpolate(float t)
{
    return (6 * t * t * t * t * t) - (15 * t * t * t * t) + (10 * t * t * t);
}

float mix(float x, float y, float alpha)
{
    return (1 - alpha) * x + alpha * y;
}

vec3 permute(vec3 x)
{
    return mod(((x*34.0)+1.0)*x, 289.0);
}

// p must be normalized!
float perlin(vec2 pos)
{
    // Cell corner positions
    vec2 bl = floor(pos);
    vec2 br = bl + vec2(1, 0);
    vec2 ul = bl + vec2(0, 1);
    vec2 ur = bl + vec2(1, 1);

    // Difference vectors
    vec2 a = pos - bl;
    vec2 b = pos - br;
    vec2 c = pos - ul;
    vec2 d = pos - ur;

    // Dot products
    float s = dot(getRandomGradient(bl), a);
    float t = dot(getRandomGradient(br), b);
    float u = dot(getRandomGradient(ul), c);
    float v = dot(getRandomGradient(ur), d);

    // Interpolation
    float st = mix(s, t, interpolate(fract(pos.x)));
    float uv_f = mix(u, v, interpolate(fract(pos.x)));
    return mix(st, uv_f, interpolate(fract(pos.y)));
}
/*float RidgedMultifractal( vec2 point, float H, float offset, float gain )
{
    float result , signal, weight, noise;
    int i;
    float exponent_array[100];

    for (i=0; i<=octavesUni; i++) {
        exponent_array[i] = pow( frequencyUni, -H );
        frequencyUni *= persistance;
    }

    signal = perlin(point);
    if (signal < 0.0)
        signal = -signal;

    signal = offset - signal;
    signal *= signal;
    result = signal;
    weight = 1.0;

    for(int i = 1; i < octaves; i++) {
        point.x *= persistance;
        point.y *= persistance;

        weight = signal * gain;
        if (weight > 1.0)
            weight = 1.0;
        if (weight < 0.0)
            weight = 0.0;

        signal = perlin(point);
        if (signal < 0.0)
            signal = -signal;

        signal = offset - signal;
        signal *= signal;
        signal *= weight;

        result += signal * exponent_array[i];
    }
    return result;
}*/
float fbmB (in vec2 st) {
    // Initial values
    float value = 0.0;
    float amplitude = amplitudeUni;
    float frequency = frequencyUni;
    int octaves = octavesUni;

    // Loop of octaves
    for (int i = 0; i < octaves; i++) {
        value += amplitude * perlin(st);
        st *= frequency;
        amplitude *= 0.5;       // change amplitude change here
    }
    return value;
}

float fbm(vec2 x) { // fractional Brownian motion
    float v = 0.5;      // parameters to vary
    float a = 0.25;     // parameters to vary
    vec2 shift = vec2(100);     // parameters to vary
    // Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));      // parameters to vary
    for (int i = 0; i < octavesUni; ++i) {
            v += a * perlin(x);
            x = rot * x * 2.0 + shift;
            a *= 0.5;
    }
    return v;
}

float fbmClass(vec2 x)
{
    float value = 0.0;

     // inner loop of fractal construction
    for (int i = 0; i < octavesUni; i++) {
        value += perlin(x) * pow(lacunarityUni, -hUni*i);
        x *= lacunarityUni;
    }
    return value;
}

void main() {
   //terrain = vec3(fbmClass(uv),fbmClass(uv),fbmClass(uv));
      //terrain=vec3(fbmClass((uv+1.0)/2.0));
      //terrain=vec3(RidgedMultifractal(2 * uv + grid_position.xy, 1.0, 1.0, 2.0) - 0.35);
      terrain = vec3(fbmClass(uv+vec2(offset_x,offset_y)));
   // Converting (x,y,z) to range [0,1]
   //float x = gl_FragCoord.x/window_width;
   //float y = gl_FragCoord.y/window_height;
   //terrain = vec3(fbmClass((vec2(x,y) * vec2(2.0f)) - vec2(1.0f)));
}


