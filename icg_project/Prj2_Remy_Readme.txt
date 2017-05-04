<<<<<<< Updated upstream
Water surface - Remy
=======
Water surface - Rémy
>>>>>>> Stashed changes

We added the surface plane for the water, with a texture mix between blue and a "sea" texture. The water surface is animated with a similar code than with the homework 2. There is a composition of a stationnary and a progressive wave, which characteristic values (amplitude, frequency and wave vector) can be easily changed with variables.
The reflection of the water is pending, waiting for the skybox to be finalized.

<<<<<<< Updated upstream
Tesselation shader / LOD - Remy
=======
Tesselation shader / LOD - Rémy
>>>>>>> Stashed changes

Added tessellation_cshader and tessellation_eshader with corresponding code. Completed the icg_helper file to be able to load the shaders. But display bugs. In progress...

+ minor corrections from previous parts - Remy

Textures + camera - Cyril

I have added several textures as follow:
- Add textures from internet (textures.com and http://image.online-convert.com/convert-to-tga)
- Load textures in grid.h and pass them to the fragment shaders as different sampler2D variables.
- Increase their "granularity" by multiplying the uv value of the vec3 variables color_rock and color_grass by some coefficients
- Blend them together using a function such as:

contibution_tex=clamp((1 - PEAK * (z- MAX) * (z - MAX)),0,1);

This yields a quadratic function with results in the interval {0,1}, having a highest contribution (1) at the MAX and peaking more strongly for higher PEAK values.

In addition, I have implemented camera controls using the keyboard and mouse, mainly based on the following tutorial: https://learnopengl.com/#!Getting-started/Camera

Skybox - Stephane
To implement the skybox, we use a cubemap on which we display our sky textures, 6 in that case for each face of the cube. For that we define as usual 
the vertices for our cube, and we correctly target each texture to the right face of the cube. We don't need to set texture coordinates for this, since 
we're going to be using our vertices positions to sample the cubemap texture we defined. For that we pass the vertices coordinates to the texture coordinates 
directly in the skybox vertex shader, and then sample the cube map texture using these texture coordinates in the fragment shader. We draw the skybox first in the background,
and we also make sure to pass the view matrix without the translation component(by switching from mat4 to mat3 and then to mat4 again) so that the skybox would'nt move 
with our camera.

Reflection - Stephane (Implementation still ongoing)
For the reflection over water, we have our view direction vector, and the normal vector to our water, which in our case is vec3(0.0, 1.0, 0.0) in local coordinates,
we transform these vectors using model view matrices, and we calculate the reflection vector in the water fragment shader, using the opengl reflect function,
using this vector to sample whichever this vector hits in our landscape thus reflecting it.