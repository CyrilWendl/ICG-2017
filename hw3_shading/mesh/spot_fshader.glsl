#version 330
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

in vec3 light_dir;
in vec3 view_dir;
in vec3 normal_mv;

out vec3 color;

const float spot_cos_cutoff = 0.1; // cos 50 degrees
const float spot_exp = 1500;

void main() {
    color = vec3(0.0,0.0,0.0);

    vec3 ambient = La * ka;
    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    float lambert = dot(n,l);
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);
    float spotEffect=0.0;
    if(lambert > 0.0) {
        diffuse = Ld*kd*lambert;
        vec3 v = normalize(view_dir);
        vec3 r = reflect(-l,n);
        specular = Ls*ks*pow(max(dot(r,v), 0.0), alpha);
        spotEffect=pow(dot(v,l),spot_exp);
        if(spotEffect>spot_cos_cutoff){
            color += spotEffect*(ambient + diffuse + specular);
        }
    }
}
