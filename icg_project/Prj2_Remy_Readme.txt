Water surface - R�my

We added the surface plane for the water, with a texture mix between blue and a "sea" texture. The water surface is animated with a similar code than with the homework 2. There is a composition of a stationnary and a progressive wave, which characteristic values (amplitude, frequency and wave vector) can be easily changed with variables.
The reflection of the water is pending, waiting for the skybox to be finalized.

Tesselation shader / LOD - R�my

Added tessellation_cshader and tessellation_eshader.
In progress...


+ minor corrections from previous parts

Textures + camera - Cyril

I have added several textures as follow:
- Add textures from internet (textures.com and http://image.online-convert.com/convert-to-tga)
- Load textures in grid.h and pass them to the fragment shaders as different sampler2D variables.
- Increase their "granularity" by multiplying the uv value of the vec3 variables color_rock and color_grass by some coefficients
- Blend them together using a function such as:

contibution_tex=clamp((1 - PEAK * (z- MAX) * (z - MAX)),0,1);

This yields a quadratic function with results in the interval {0,1}, having a highest contribution (1) at the MAX and peaking more strongly for higher PEAK values.

In addition, I have implemented camera controls using the keyboard and mouse, mainly based on the following tutorial: https://learnopengl.com/#!Getting-started/Camera
