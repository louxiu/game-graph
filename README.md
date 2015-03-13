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

* screenshot
  1. view1~6
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/main.png)
  7. view7 light
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view7.png)
  8. view8 fps
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view8.png)
  9. view9 earth
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view9.png)
  10. view10 3d texture
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view10.png)
  11. view11
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view11.png)
  12. view12
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view12.png)
  13. view13
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view13.png)
  14. view14 3d math pic
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view14.png)
  15. view15 3d math pic
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view15.png)
  16. view16 teapot
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view16.png)
  17. view17 mask
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view17.png)
  18. view18 text
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view18.png)
  19. view19 select
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view19.png)
  20. view20 aeroplane chess
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view20.png)
  21. view21 portal
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view21.png)
  22. view23 point cloud
  ![ScreenShot](https://raw.githubusercontent.com/louxiu/game-graph/master/data/view23_pcd5.png)
