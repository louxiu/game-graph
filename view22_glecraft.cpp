#include <iostream>
#include <vector>
using namespace std;

#include "view22_glecraft.h"
#include "util.h"

#include "engine/Mesh.h"
#include "engine/Program.h"
#include "engine/Render.h"

static Program *program = NULL;
static Mesh *mesh = NULL;
static Render *render = NULL;

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
