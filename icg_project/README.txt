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

* FPS implemented *

* Infinite terrain implemented *

Get texture to work (shift with mountains):
- By default, the grid is stationary between -16 and 16 and the mountains move, thus it looks as though the texture
should move with the mountains. To do so, we implemented also passed the offset to the texture vshader (uv coordinates).
- If we pass it to the fshader as follows:
uv = offset+(position + vec2(16.0)) / 32.0f;


-Water Reflection
Camera position mirror
Texture mirror in water header
Render to framebuffer
Query framebuffer texture

-Skybox
Rotation by rotate view matrix of skybox
Multiple textures, only 2 sampler cubes for blending effect between cycles, and then pass the correct texture in active textures
Modify diffuse for terrain and water depending on time of day


