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