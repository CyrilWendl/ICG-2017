Wire frame triangle grid - Rémy

For this part, we took the code from homework 2, and created a triangle grid of 512*512 vertices. This code can be found in the grid.h.

Grid textured with Perlin noise - Rémy

For applying the texture, we made use of a quad and a framebuffer. We initialized the grid in the main.cpp with as argument the texture of the framebuffer. Then we draw a quad in the framebuffer, with the texture specified in quad_fshader.glsl. This texture is the perlin noise. Instead of converting the code given as an advice, we looked in Google for a glsl version of the Perlin noise, which we found in this repository: https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83 . We finally get a grid textured with a quad, which is textured with the Perlin noise.

Height map - Rémy

For the height map, we then had to just put a condition on the pixel color in the grid_fshader.glsl, and adjust a new z-coordinate according to the color of the height map (the more it's white, the higher it is). We also had to divide this height by an arbitrary scaling factor to make it look nicer.

Fractional brownian motion - Rémy

For this part, it was only needed to use the Perlin noise in a fBm algorithm. We directly implemented the one given in the lecture slides, and had to adjust the parameters to get a mountain looking more or less like the ones in the screenshots. We also tested other fBm algorithms found on the same repo as the Perlin noise to see how they fitted.

Diffuse shading - Stephane
For the diffuse shading, we introduced a light structure with the diffuse component in the grid header. And then we caluclate said diffuse shading in the grid fshader, by calculating the normal to the surface as per the flat shader in the homework (the cross product of the gradients). We also introduce a custom material component coefficient kd that should depend on the surface and thus on the height of our terrain.

Trees(Work in progress) - Stephane
Started looking into L-systems and recursive functions for generating trees, and how to populate our terrain with them. 
