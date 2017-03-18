#version 330

in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

out vec3 color;


void main() {
    color = vec3(0.0,0.0,0.0);
//    const vec3 COLORS[6] = vec3[](
//        vec3(1.0,0.0,0.0),
//        vec3(0.0,1.0,0.0),
//        vec3(0.0,0.0,1.0),
//        vec3(1.0,1.0,0.0),
//        vec3(0.0,1.0,1.0),
//        vec3(1.0,0.0,1.0));
//    int index = int( mod(gl_PrimitiveID,6) );
//    color = COLORS[index];

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 4.2: Flat shading.
    /// 1) compute triangle normal using dFdx and dFdy
    vec3 n = normalize(cross(dFdx(vpoint_mv.xyz),dFdy(vpoint_mv.xyz)));     // take the plane x,y vectors (using dFdx,y), calculate cross product, and normalize, thisshould give the normal
    /// 1) compute ambient term.
    color += La*ka;
    /// 2) compute diffuse term.
    float cosDiffuse = dot(n,light_dir);
    if (cosDiffuse > 0.0)
    {
        color += Ld*kd*cosDiffuse;
        vec3 r = reflect(-light_dir, n);
        color += Ls*ks*pow(max(dot(r,view_dir), 0.0), alpha);
    }
    /// 3) compute specular term.
    ///<<<<<<<<<< TODO <<<<<<<<<<<
}
