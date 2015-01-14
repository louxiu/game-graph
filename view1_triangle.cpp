#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

#include "view1_triangle.h"
#include "Mesh.h"
#include "util.h"

Mesh view1_triangle_mesh;
GLint view1_attr_coord2d = -1;
GLuint view1_triangle_program;

void view1TriangleDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(view1_triangle_program);

    view1_triangle_mesh.draw(view1_triangle_mesh,
                             view1_attr_coord2d,
                             -1, -1, -1, -1, -1, -1);

    // testOnDisplay();

    glutSwapBuffers();
}

int view1_triangle_initResources()
{
    glClearColor(1.0, 1.0, 1.0, 0);

    glm::vec4 triangle_vertices[3] = {
        glm::vec4(0.0, 0.8, 0.0, 1.0),
        glm::vec4(-0.8, -0.8, 0.0, 1.0),
        glm::vec4(0.8, -0.8, 0.0, 1.0)
    };

    for(int i = 0; i < sizeof(triangle_vertices); ++i){
        view1_triangle_mesh.vertices.push_back(triangle_vertices[i]);
    }

    view1_triangle_mesh.upload();

    view1_triangle_program = create_program("glsl/triangle.1.v.glsl",
                                            "glsl/triangle.1.f.glsl");

    view1_attr_coord2d = get_attrib(view1_triangle_program, "coord2d");

    // test_initResources();

    return 0;
}

void view1_triangle_freeResources()
{
    glDeleteProgram(view1_triangle_program);
}

void view1_entry(Window *window)
{
    window->program = view1_triangle_program;
    window->display = view1TriangleDisplay;
    window->entry = viewEntry;
    window->init = view1_triangle_initResources;
    window->free = view1_triangle_freeResources;
}

#ifdef TEST_ALONE
int main(int argc, char *argv[])
{
    Window window;
    resetWindow(&window);

    view1_entry(&window);

    if (mini_initWindow(argc, argv, &window) == 0){
        glutMainLoop();
    }

    return 0;
}
#endif /* TEST_ALONE */
