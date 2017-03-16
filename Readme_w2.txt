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
