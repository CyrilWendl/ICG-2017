**WEEK 1**

*Ex 1: spirals*

1. Simple spiral
The spirals were created in a simple polar coordinate system, with rotation and angle as indicators for the positions of the triangle. For the first spiral, we simply implemented an Archimedean spiral, with the following formula:

r = a + b*theta

with theta being the angle, r the radius, a being the initial rotation of the spiral and b  a size parameter for adjusting the spacing of circles of the spiral.

The location and rotation of each triangle was then expressed in parametric form, as:

x(t) = R cos(t), y(t) = R sin(t) [1]

2. Fermat’s spiral

We used a similar formula but this time without the rotation and spacing parameters a and b, but just theta:

r=+-theta^(1/2)

with 
- angle theta = n*137.508°, n being the index number of the triangel
- rotation r=c*sqrt(n), c being a spacing factor

The location of rotation of spirals was then implemented in parametric form as above.

[1] http://www.mathematische-basteleien.de/spiral.htm

*Ex 2: checkerboard *
As suggested by the exercise handout, we use a sine wave to interpolate the colours and form the colored checkerboard. We see that the quad we're manipulating has a range of values 0 to 1. So we're remapping the [0,1] to [0, 2Pi] to effectively use the sine wave interpolation, while truncating negative values from the sine so we get valid colors from the color map provided. The checkerboard is 10x10 squares, with the interpolation acting over two consecutive squares. So basically we need to set the frequency of the sine wave to 5 so we cant get the right pattern to 10 squares. We do this twice, one for the X axis of the checkerboard and the other for the Y axis and we combine.

*Ex 3: planets *

What we had to do was just to apply the right transformation matrices in the right order to the planets.
For the Sun:
	- A rotation matrix (to make it rotate around itself)
	- And then a scaling matrix, which also translates the sun on the right of the window, to be able to see the ellipsoidal trajectory of the Earth.

For the Earth:
	- A rotation matrix
	- A scaling matrix
	- A translation matrix with ellipsoidal motion, such that the sun appears as a focus.

For the Moon:
	- Rotation matrix
	- Scaling matrix
	- Translation matrix with ellipsoidal motion, centered around the Earth.

To make to speed tweak (acceleration due to the other bodies), we just multiplied the speed of translation by a factor of (1-distanceBetweenTwoBodies), the distance being calculated based on the current translations matrices values.


**WEEK 2**
*Ex 1: Projection *

A new projection mat4 PerspectiveProjection was created to implement the OpenGL Perspective Projection Matrix as specified here: http://www.songho.ca/opengl/gl_projectionmatrix.html#perspectiv,

The projection was then created via projection_matrix = PerspectiveProjection(-right, right, -top, top, 3.0f, -3.0f), similar to the orthographic projection created in class. Notice the change in the near and far coordinates, which have inverted signs compared to the initialization of the orthogonal projection matrix. This is due to the fact that we map each point to a cube, using a left-handed coordinate system.

*Ex 2: Trackball *
First we implement the function ProjectOntoSurface to be able to project points onto the sphere surface, by correctly calculating the corresponding z coordinate in terms of x, y and the sphere radius. Then to calculate the rotation matrix in function of the dragged mouse cursor in the Drag function, we take the cross product and the arcos of the dot product of the anchor position and the current position to get the rotation axis and the magnitude of rotation respectively. Finally we get to apply all this in the main program and get the trackball. For the zoom, we keep a zoom factor that we use to translate the view_matrix, the former being updated by the difference in the y coordinate of the mouse cursor.


*Ex 3: Waves *
Triangle meshing:

We created the vertices starting from bottom left, upwards. Then the indices were assigned in the same order: for each vertex found, we make two triangles from it, on its right. Then we have to exclude the top and right borders vertices.

For the animation we created a stationnary wave along x and y, varying in time. To make it look like water, we added another stationnary wave, plus an excitation wave (propagating wave). The result is quite resembling, if you think about a water pond that you excite throwing in some pebbles for example.


**WEEk 3**
*Ex 1: Phong shading*
We compute the normal, the light and view direction and pass them to the fragment shader, where we compute the ambient, the diffuse and the specular terms to get the intensity. This allows us to get the intensity per vertex normal.

*Ex 2: Toon shading*
We use the same vertex shader as in phong vertex shader. In the toon fragment shader, we evaluate the ambient term as usual, but for the diffuse and the specular, we use the values of the dot products of our original formula to be used as coordinates to get the corresponding discrete value from the provided 1dTexture and plug it in the original formula for each of the diffuse and the specular parts. (After binding the textures of course in mesh.h)

*Ex3: Artistic shading*
The idea is as before. But in this case we bind a 2dTexture to artistic shader, and both the dot products will have to be used (as a vec2) as coordinates to access the value in the 2D texture provided.
