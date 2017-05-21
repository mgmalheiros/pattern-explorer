# pattern-explorer
Reference implementation and experiments for the model described on the paper *Pattern formation through minimalist biologically inspired cellular simulation*, presented at the Graphics Interface 2017 conference.

There is a simple Makefile, that needs to be updated before compiling. It has been only tested on Linux, but should be fairly simple to port to Mac OS or Windows.

There are three executables:

  * **pattern**: GUI program, with all features
  * **offline**: command-line only version, made for benchmarking purposes
  * **simple**: simple example of defining the initial state and running the simulation through the API (does not parse experiment files)

Dependencies:

  * [AntTweakBar](http://anttweakbar.sourceforge.net/), for the user interface
  * [CGAL](http://www.cgal.org/), for the natural neighbor coordinate computation, which produces high-quality output textures
  * [GLUT](http://freeglut.sourceforge.net/), as simple platform-independent windowing system for OpenGL
  * [LibPNG](http://www.libpng.org/), for PNG output support
  * [GLM](http://glm.g-truc.net/), for utility mathematical functions

Key bindings for GUI program:

  * [space] run a single iteration
  * [tab] run at most 50 iterations
  * [esc] quit
  * [A] change shown chemical
  * [B] show/hide information bar
  * [C] center pattern in view
  * [D] show/hide domain limits
  * [L] reload colormap definitions
  * [M] change colormap type
  * [N] show/hide nearest neighbors for current cell (picked a right-click)
  * [O] output a screenshot
  * [P] show/hide polarity vectors
  * [S] start/stop simulation
  * [T] outupt high-quality interpolated texture
  * [X] change cell exhibition type

