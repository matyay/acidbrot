# Acid-brot

An interactive psychodelic image generator based on the Mandelbrot and the Julia set fractals.
Allows realtime navigation through the fractal along with some cool visual effects.

## Installation (building)

Currently tested only on Linux sysem (Ubuntu 18.04)

Prerequisites:

- libglfw3 (v3.2+)
- libX11
- libpng

Installation:
```
cmake .
make -j acidbrot
```

Running:
```
./acidbrot
```

## Navigation

Keyboard:

|Key(s)|Action|
|---|---|
|Arrows|Lateral camera movement|
|A/D|Rotation left/right|
|W/S|Zoom in/out|
|Z/C|Cycle colors|
|F|Switch between Mandelbrot / Julia set|
|Q/E|Change Julia set `angle(c)` value|
|1/3|Change Julia set `abs(c)` value|
|F12|Save a screenshot|
|Alt+Enter|Switch between fullscreen and windowed mode|
|F1-F8|Change window size (and resolution)|
|Home/End|Select a parameter to modify (name and value shown in the upper-left corner|
|PgUp/PgDn|Adjust the selected parameter value|
|Esc|Exit the application|

## Future plans

- More post processing effects
- Camera control through a joystick etc.
- Ability to save motion a path and re-play it.
- FX controlled by sound (music) ?