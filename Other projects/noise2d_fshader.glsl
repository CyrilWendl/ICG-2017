#version 330 core

in vec2 uv;
out float height;

uniform int noise;
uniform int fractal;
uniform vec3 grid_position;

uniform float time;
uniform int octaves;
uniform float persistance;
uniform float frequency;

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

float simplexNoise(vec2 v )
{
    const vec4 C = vec4(0.211324865405187, 0.366025403784439,
                        -0.577350269189626, 0.024390243902439);
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    i = mod(i, 289.0);
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
                      + i.x + vec3(0.0, i1.x, 1.0 ));
    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
                            dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

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

float octave()
{

    float total = 0.f;
    float frequency = 1.f;
    float amplitude = 1.1f;

    for (int i = 0; i < octaves; i++) {
        vec2 value = (2 * uv + grid_position.xy) * frequency;

        if (noise == 1)
            total += simplexNoise(value) * amplitude;
        else
            total += perlin(value)*amplitude;

        amplitude *= persistance;
        frequency *= 2.f;
    }

    return total;
}

float RidgedMultifractal( vec2 point, float H, float offset, float gain )
{
    float result, frequency, signal, weight, noise;
    int i;
    float exponent_array[100];

    frequency = 1.0;
    for (i=0; i<=octaves; i++) {
        exponent_array[i] = pow( frequency, -H );
        frequency *= persistance;
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
}

void main()
{
    if (fractal == 0)
        height = octave() + 0.5;
    else
        height = RidgedMultifractal(2 * uv + grid_position.xy, 1.0, 1.0, 2.0) - 0.35;
}
