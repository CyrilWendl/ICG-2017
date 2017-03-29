**WEEk 4**
*Ex 1: Efficiant Gaussian Blurring*
For this exercise, several attachments were added to the framebuffer. First, the file framebuffer.h was modified to accept several texture attachments, which could be added by the corresponding attributes „layout (location=0) out color“(=default case) or „layout (location=1) out color“ (see fshaders of the cube, quad and screenquad). 

In main.c, the framebuffer bind function is called a second time and the screenquad is drawn with the parameters stdev and pass (0 or 1), which defines whether the image is blurred vertically or horizontally. The „int pass“ argument of the function Draw is also given to the fshader as an uniform.  In screenquad.h, two textures are defined and passed to the fshader of the screenquad as uniforms named „tex“ and „tex1“. A kernel is calculated based on the std input of the Draw() function, and passed on as a uniform to the fshader as well. The kernel is calculated as follows:

e^{-x^2/(2*std^4)} [1]

In the fshader of the screenquad, the uniform pass is used to decide on whether to render the first color texture or the second one, blurring either horizontally or vertically the image.

[1] Slightly changed from this source: http://www.stat.wisc.edu/~mchung/teaching/MIA/reading/diffusion.gaussian.kernel.pdf.pdf

*Ex 2: Screen space reflections
In this exercise, we had to make believe the user that the cube is reflected on the floor. To do so, we started by rendering the cube "upside down" by displacing the camera, and passing this cube to the framebuffer (bind/unbind). Then in the framebuffer, it was needed to get the Fragcoord of the current pixel, and normalize it by the window dimensions, to have the coordinates in the [0,1] frame. Then we mix the texture of the floor with the one of the inverted cube, with a certain level of transparency. Before that, the floor has been initialized with the framebuffer_texture_id. Tadaam!

*EX 3: Motion Blur*
We have our projected current and previous positions in homogeneous coordinates. To be able to compute the veloctiy vector we need to switch back from homogeneous coordinates so we divide by the w coordinate the vectors and take the xy coordinates(since we don't really need z for this motion). Finally we get the velocity by substraction of those two. We save it to the velocity texture. Finally, we use this veloctiy texture in screenquad and do a directional convolution on it. The bigger the N we use for the convolution, the more the points we get, the better the blur.
