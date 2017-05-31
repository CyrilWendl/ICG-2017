Part 3
** Cyril **
* Camera improved *
The following variables determine the movement timing:
- Time variables pressTime, releaseTime: updated on press and release of a key
- Time differences pressedTime and timeDiff:
    - pressedTime: releaseTime - pressTime. Updated on release of the key that was pressed last. Limited to 3 seconds
    - timeDiff: the difference in time since a key was last pressed. Updated every frame.

Using these variables, the camera speed intensity is calculated as follows after releasing all buttons:
intensity = max((pressedTime - timeDiff)/pressedTime,0).
With pressedTime going from 0 to 3, timeDiff from 0 to infty, we get an intensity between 1, right after releasing the buttons, to 0, when timeDiff exceeds pressedTime.

Situations:
1. One key
- The key is pressed. The variable key of the corresponding key is set to true. The corresponding camera movement in move_terrain() is executed.
- The key is released. The elapsed time between press and release is stored in pressedTime.
- Time since releasing is measured and stored in timeDiff. While timeDiff (updated every frame) is smaller than the pressTime (set only after release), the intensity is above 0, going from 1 (timeDiff=0) to 0 (timeDiff=pressedTime). Multiply the camera speed by the intensity.

2. Several keys
- First key: key variable set to true
- Second key: key_callback called a second time. The second key is set to true. All movements all executed in full intensity
- On releasing the keys, the durion of the key that was pressed last is used to make a smooth-out transition.
- If all keys are released, only the movement of the last key is executed smoothly until intensity is zero.

* Infinite terrain implemented *
The infinite terrain was done with a simple trick: instead of moving the camera, pass an offset to the grid shader and use this to calculate the Perlin noise. The offset corresponds to the former x- and y-coordinates of the camera eye and is passed to the terrain as follows:

terrain=vec3(RidgedMultifractal(2 * uv + offset, 1.0, 1.0, 2.0) - 0.8);

* Implemented rigged multifractal *


* FPS implemented *
The FPS camera was implement by retrieving the pixel in the middle of the terrain and binding the camera's height to it:
glReadPixels(TEX_WIDTH/2,TEX_HEIGHT/2,1,1, GL_RED, GL_FLOAT, tex);
if(cameraMode==CAM_FPS){
     if(tex[0]<.2)
         tex[0]=.2;
     cameraPos.y=tex[0]+.5;
 }
 The if-condition assures the camera stays smoothly above the water.


Get texture to work (shift with mountains):
- By default, the grid is stationary between -16 and 16 and the mountains move, thus it looks as though the texture
should move with the mountains. To do so, we implemented also passed the offset to the texture vshader (uv coordinates).
- If we pass it to the fshader as follows:
uv = offset+(position + vec2(16.0)) / 32.0f;

* Added GUI *

* Water Reflection and Refraction
- We use two framebuffers to which we render the Reflection and Refraction textures.
- We draw the grid and sky for the reflection buffer, and only the grid for the refraction buffer
- For the reflection, we invert the camera position and its pitch, and we create a view mirror matrix that we use to draw for the reflection buffer.
- We query the framebuffers textures. For that, we calculate the NDC coordinates(using gl_frag coord) to sample the two textures from in the water fshader.

* Water height in GUI
- We parametrize the water height in our code with all the required adjustments. We can control it with the GUI.

* Clipping the grid
- To get the correct the reflection and refraction, we have to clip the correct part of the terrain for that. For instance, for reflection we have to clip all that's under the water,
and render all that's above it. We do the opposite for the refraction. For that we add clip flags as arguments to the grid draw function, so if we draw
to the reflection framebuffer, we set reflectflag to 1; that clips all that's under the water(using glclipdistance0 in grid v shader). So if we draw the original
terrain, we set both the reflect and refract clip flags to 0 so that we draw the whole grid (We don't clip anything).

*Skybox UPDATES
- Apply a Rotation to the view matrix of the skybox (To get the effect of clouds moving)
- We implement a day and night cycle (3 cycles: Day, Sunset, Night). For that we use 3 different cubemap sky textures, one for each cycle.
- For the transition between cycles, we use a blending uniform variable that we pass to the skybox shaders, so that we mix the two textures on hand for that
time of day, taking that blending variable as a mixing factor. As time passes from day to sunset for instance, the blending increases from 0 to 1 depending
on the passing time. When it's zero it's full day, and when it gets to 1 it's full sunset and in between is a smooth transition from day to sunset. (For 
that we map [0, 1] to [day_end, sunset_start] for this example).
- Since we only need two sampler cubemap in the skybox fshader (for the blending effect), now it's a matter of passing which textures to the shader,
and for that we control the active textures depending on time in the skybox header.  
- We can control the pacing of the cycles by declaring another uniform variable for that, and we add it to the GUI.
- We also modify the lighting of the water and the grid using the diffuse lighting. For that we use a uniform diffuse_factor that
changes depending on the time of day and we do a similar mapping technique as the blending effect for that.

** Rémy **
* Unsuccessful implementation of Level-Of-Detail and Tessellation *

* Animated trees implemented *
Trees placed randomly on the terrain, and animated with "wind" (sine perturbation around equilibrium position). Height and width of the trees vary also.
