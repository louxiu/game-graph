#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <GL/glew.h>
#include <GL/glut.h>

#ifndef GLM_GTC_noise
#define GLM_GTC_noise GLM_VERSION
#endif

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include "view22_glecraft.h"
#include "util.h"

void view22_entry(Window *window)
{
    // window->display = view22TriangleDisplay;
    // window->entry = viewEntry;
    // window->init = view22_triangle_initResources;
    // window->free = view22_triangle_freeResources;
}

#ifdef TEST_ALONE
int main(int argc, char *argv[])
{
    Window window;
    resetWindow(&window);

    view22_entry(&window);

    if (mini_initWindow(argc, argv, &window) == 0){
        glutMainLoop();
    }

    return 0;
}
#endif /* TEST_ALONE */
