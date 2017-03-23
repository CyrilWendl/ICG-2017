#version 330

in vec3 light_dir;
in vec3 view_dir;
in vec3 normal_mv;

out vec3 color;

uniform float alpha;
uniform sampler2D tex2D;

void main() {
    color = vec3(0.0, 0.0, 0.0);

    /**const vec3 COLORS[6] = vec3[](
        vec3(1.0,0.0,0.0),
        vec3(0.0,1.0,0.0),
        vec3(0.0,0.0,1.0),
        vec3(1.0,1.0,0.0),
        vec3(0.0,1.0,1.0),
        vec3(1.0,0.0,1.0));
    int index = int( mod(gl_PrimitiveID,6) );
    color = COLORS[index];*/

    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 3.2: Artistic shading.
    // 1) compute the output color by doing a look-up in the texture using the
    //    texture sampler tex.
    //<<<<<<<<<< TODO <<<<<<<<<<<
    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    float lambert = dot(n,l);
    vec4 text_value = vec4(0.0, 0.0, 0.0, 0.0);
    if(lambert > 0.0) {
        vec3 v = normalize(view_dir);
        vec3 r = reflect(-l,n);
        vec2 tex_coord = vec2(lambert, pow(max(dot(r,v), 0.0), alpha));
        text_value = texture(tex2D, tex_coord);
    }

    color += text_value.rgb;
}

