#include <iostream>
#include <vector>
using namespace std;

#include "view2_triangle.h"
#include "util.h"

#include "engine/Mesh.h"
#include "engine/Program.h"
#include "engine/Render.h"

static Program *program = NULL;
static Mesh *mesh = NULL;
static Render *render = NULL;

int view2_triangle_initResources()
{
    mesh = new Mesh();

    glm::vec4 triangle_vertices[3] = {
        glm::vec4(0.0, 0.8, 0.0, 1.0),
        glm::vec4(-0.8, -0.8, 0.0, 1.0),
        glm::vec4(0.8, -0.8, 0.0, 1.0)
    };

    for(int i = 0; i < sizeof(triangle_vertices); ++i){
        mesh->vertices.push_back(triangle_vertices[i]);
    }

    mesh->set_attr_v_name("coord2d");
    mesh->upload();

    program = new Program("glsl/triangle.2.v.glsl",
                          "glsl/triangle.2.f.glsl");

    render = new Render(mesh, program);

    return 0;
}

void view2TriangleDisplay()
{
    glClearColor(1.0, 1.0, 1.0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    render->begin();
    render->draw();
    render->end();

    glutSwapBuffers();
}

void view2_triangle_freeResources()
{
    delete program;
    delete mesh;
    delete render;
}

// TODO: add boost as feature, auto free?

void view2_entry(Window *window)
{
    window->display = view2TriangleDisplay;
    window->entry = viewEntry;
    window->init = view2_triangle_initResources;
    window->free = view2_triangle_freeResources;
}

#ifdef TEST_ALONE
int main(int argc, char *argv[])
{
    Window window;
    resetWindow(&window);

    view2_entry(&window);

    if (mini_initWindow(argc, argv, &window) == 0){
        glutMainLoop();
    }

    return 0;
}
#endif /* TEST_ALONE */
