**WEEk 3**
*Ex 1: Phong shading*
We compute the normal, the light and view direction and pass them to the fragment shader, where we compute the ambient, the diffuse and the specular terms to get the intensity. This allows us to get the intensity per vertex normal.

*Ex 2: Toon shading*
We use the same vertex shader as in phong vertex shader. In the toon fragment shader, we evaluate the ambient term as usual, but for the diffuse and the specular, we use the values of the dot products of our original formula to be used as coordinates to get the corresponding discrete value from the provided 1dTexture and plug it in the original formula for each of the diffuse and the specular parts. (After binding the textures of course in mesh.h)

*Ex3: Artistic shading*
The idea is as before. But in this case we bind a 2dTexture to artistic shader, and both the dot products will have to be used (as a vec2) as coordinates to access the value in the 2D texture provided.

*Ex4: Flat shading*
The main difficulty here was to calculate the normals of each triangle, using dFdx and dFdy. These functions give the local partial derivative with respect to x and y respectively. So if one applies these derivatives at the center of a plane triangle, it will give the slope on x and on y, in other words we can find the plane vectors. We then just have to compute the cross product to calculate the third vector, orthogonal to the plane, and normalize it to get the normal. :) 

*Ex5: Spotlight shading*
Here we use the same idea as for the Phong shading, but multiplying the color by a spot effect, which is calculated as the dot product between the normalized light direction and the normalized viewing direction, to the power of some exponent.

