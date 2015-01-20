#include <iostream>
#include <vector>
using namespace std;

#include "view4_triangle.h"
#include "util.h"

#include "engine/Mesh.h"
#include "engine/Program.h"
#include "engine/Render.h"

static Program *program = NULL;
static Mesh *mesh = NULL;
static Render *render = NULL;

void view4TriangleDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);

    float move = sinf(glutGet(GLUT_ELAPSED_TIME) / 1000.0 * (2*3.14) / 5);
    float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 45;
    glm::vec3 axis_z(0, 0, 1);
    glm::mat4 m_transform =
        glm::translate(glm::mat4(1.0f), glm::vec3(move, 0.0, 0.0)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_z);

    program->set_uniformMatrix4fv("m_transform", 1, GL_FALSE,
                                  glm::value_ptr(m_transform));

    render->draw();

    glutSwapBuffers();
}

int view4_triangle_initResources()
{
    glClearColor(1.0, 1.0, 1.0, 0);

    program = new Program("glsl/triangle.4.v.glsl",
                          "glsl/triangle.4.f.glsl");

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
    program->bind_mesh(mesh);

    render = new Render(mesh, program);

    return 0;
}

void view4_triangle_freeResources()
{
    program->unbind_mesh(mesh);

    delete program;
    delete mesh;
    delete render;
}

void view4_entry(Window *window)
{
    window->display = view4TriangleDisplay;
    window->entry = viewEntry;
    window->init = view4_triangle_initResources;
    window->free = view4_triangle_freeResources;
}

#ifdef TEST_ALONE
int main(int argc, char *argv[])
{
    Window window;
    resetWindow(&window);

    view4_entry(&window);

    if (mini_initWindow(argc, argv, &window) == 0){
        glutMainLoop();
    }

    return 0;
}
#endif /* TEST_ALONE */
