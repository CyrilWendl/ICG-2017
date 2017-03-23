#version 330
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

in vec3 light_dir;
in vec3 view_dir;
in vec3 normal_mv;

out vec3 color;

uniform sampler1D tex1D;

void main() {
    color = vec3(0.0,0.0,0.0);

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 2.2: Toon shading.
    /// 1) compute ambient term.
    /// 2) compute diffuse term using the texture sampler tex.
    /// 3) compute specular term using the texture sampler tex.
    ///<<<<<<<<<< TODO <<<<<<<<<<<
    vec3 ambient = La * ka;
    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);
    float lambert = dot(n,l);
    if(lambert > 0.0) {
        vec4 nl_textured = texture(tex1D, lambert);
        diffuse = Ld * kd * nl_textured.r;
        vec3 v = normalize(view_dir);
        vec3 r = reflect(-l,n);
        float dot_rv = pow(max(dot(r,v), 0.0), alpha);
        vec4 rv_textured = texture(tex1D, dot_rv);
        specular = rv_textured.r * Ls * ks;
        /**if(dot_rv > 0.0) {
            vec4 rv_textured = texture(tex1D, dot_rv);
            //specular = Ls*ks*pow(max(texture1D(tex1D, dot(r,v)), 0.0), alpha);
            specular =  vec3(pow(rv_textured.r, alpha), pow(rv_textured.g, alpha), pow(rv_textured.b, alpha));
            specular = specular * Ls * ks;
        }*/
    }
    color += ambient + diffuse + specular;
}
