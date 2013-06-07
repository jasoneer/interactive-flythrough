Interactive Flythough of the Known Universe
======================
#### A 2013 UC Davis Senior Design Project ####

Using data from Sloan Digitial Sky Survey, we created an interactive 3D simulation of galaxies to scale of the known universe. Each galaxy is textured and billboarded by an image representative of its type and the size of the texture is representative to the scaled size of the galaxy.

CONTROLS:
- Quit: ESC
- Fullscreen: E
- Reset Scene: R
- Toggle Textures: T
- Forward/Backward: W,S
- Rotate Scene: Mouse Drag

PLATFORMS:
- Mac (OS X)
- Linux (Ubuntu)
- Windows

LIBRARIES:
- OpenGL 2.x
- SDL 1.2.15
- LeapMotion 0.7.9

INSTALLATION:
- Prerequisites:
    - SDL 1.2.15

- Mac/Linux:
```
initial clone (one time): make init
on x64 platforms: make
on x86 platforms: make x86
remove binary/objs: make clean
```
- Windows:
```
To Come
```
USAGE:
- From command line of Makefile:
```
bin/flythrough
```

CONTACT:
- Jason Hsu <jlhsu@ucdavis.edu>
- Charles Henderson <cdhenderson@ucdavis.edu>
- Daniel Chen <danchen@ucdavis.edu>
