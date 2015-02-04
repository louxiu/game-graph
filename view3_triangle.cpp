#include <iostream>
#include <vector>
using namespace std;

#include "view3_triangle.h"
#include "util.h"

#include "engine/Mesh.h"
#include "engine/Program.h"
#include "engine/Render.h"

static Program *program = NULL;
static Mesh *mesh = NULL;
static Render *render = NULL;

void view3TriangleDisplay()
{
    glClearColor(1.0, 1.0, 1.0, 0);
    glClear(GL_COLOR_BUFFER_BIT);


    float cur_fade = sinf(glutGet(GLUT_ELAPSED_TIME) /
                          1000.0 * (2 * M_PI) / 5) / 2 + 0.5;

    render->begin();
    program->set_uniform1f("fade", cur_fade);
    render->draw();
    render->end();

    glutSwapBuffers();
}

// TODO: pass vec4 to shader vec3
int view3_triangle_initResources()
{
    program = new Program("glsl/triangle.3.v.glsl",
                          "glsl/triangle.3.f.glsl");

    mesh = new Mesh();

    // vertices
    glm::vec4 triangle_vertices[3] = {
        glm::vec4(0.0, 0.8, 0.0, 1.0),
        glm::vec4(-0.8, -0.8, 0.0, 1.0),
        glm::vec4(0.8, -0.8, 0.0, 1.0)
    };

    for(int i = 0; i < sizeof(triangle_vertices); ++i){
        mesh->vertices.push_back(triangle_vertices[i]);
    }

    mesh->set_attr_v_name("coord2d");

    // colors
    glm::vec4 triangle_colors[3] = {
        glm::vec4(1.0, 1.0, 0.0, 1.0),
        glm::vec4(0.0, 0.0, 1.0, 1.0),
        glm::vec4(1.0, 0.0, 0.0, 1.0)
    };

    for(int i = 0; i < sizeof(triangle_colors); ++i){
        mesh->colors.push_back(triangle_colors[i]);
    }

    mesh->set_attr_c_name("v_color");

    mesh->upload();

    render = new Render(mesh, program);

    return 0;
}

void view3_triangle_freeResources()
{
    delete program;
    delete mesh;
    delete render;
}

void view3_entry(Window *window)
{
    window->display = view3TriangleDisplay;
    window->entry = viewEntry;
    window->init = view3_triangle_initResources;
    window->free = view3_triangle_freeResources;
}

#ifdef TEST_ALONE
int main(int argc, char *argv[])
{
    Window window;
    resetWindow(&window);

    view3_entry(&window);

    if (mini_initWindow(argc, argv, &window) == 0){
        glutMainLoop();
    }

    return 0;
}
#endif /* TEST_ALONE */
