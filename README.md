* http://en.wikibooks.org/wiki/OpenGL_Programming
* requirement
  1. opengl
  2. freeglut3-dev: an upper aip layer for window managment
  3. glew: used for dynamic load opengl library api
  4. glm: something about math

* linking
  -lglut -lGLEW -lGL -lglm

* glut api
  https://www.opengl.org/documentation/specs/glut/spec3/spec3.html

* install
  sudo apt-get install mesa-utils
  glxinfo and glxgears

* check card
  lspci

* problem, need to reinstall the driver
  X Error of failed request:  BadRequest (invalid request code or no such operation)
  Major opcode of failed request:  154 (GLX)
  Minor opcode of failed request:  34 ()
  Serial number of failed request:  34
  Current serial number in output stream:  33

* start gui program to desktop from command line
  export DISPLAY=:0

![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/main.png)