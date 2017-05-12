Part 3
** Cyril **
* Camera added *

* FPS implemented *

* Infinite terrain implemented *

Get texture to work (shift with mountains):
- By default, the grid is stationary between -16 and 16 and the mountains move, thus it looks as though the texture
should move with the mountains. To do so, we implemented also passed the offset to the texture vshader (uv coordinates).
- If we pass it to the fshader as follows:
uv = offset+(position + vec2(16.0)) / 32.0f;


