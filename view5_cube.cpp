#include <iostream>
#include <vector>
using namespace std;

#include "view5_cube.h"
#include "util.h"

#include "engine/Mesh.h"
#include "engine/Program.h"
#include "engine/Render.h"

static Program *program = NULL;
static Mesh *mesh = NULL;
static Render *render = NULL;


void view5CubeDisplay()
{
    // TODO: the cube transparent problem
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: perspective internal
    // TODO: move to mesh and clean this
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0));
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0),
                                 glm::vec3(0.0, 0.0, -4.0),
                                 glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 projection = glm::perspective(45.0f, 1.0f*128/128, 0.1f, 10.0f);

    float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 3;  // 5° per second
    glm::vec3 axis_y(0.0, 1.0, 0.0);
    glm::mat4 anim = glm::rotate(glm::mat4(1.0f), angle, axis_y);
    glm::mat4 mvp = projection * view * model * anim;


    program->set_uniformMatrix4fv("mvp", 1, GL_FALSE,
                                  glm::value_ptr(mvp));

    render->draw();

    glutSwapBuffers();
}

int view5_cube_initResources()
{
    glClearColor(1.0, 1.0, 1.0, 0);
    // glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    program = new Program("glsl/cube.5.v.glsl",
                          "glsl/cube.5.f.glsl");
    mesh = new Mesh();

    glm::vec4 cube_vertices[8] = {
        glm::vec4(-1.0, -1.0, 1.0, 1.0),
        glm::vec4(1.0, -1.0, 1.0, 1.0),
        glm::vec4(1.0, 1.0, 1.0, 1.0),
        glm::vec4(-1.0, 1.0, 1.0, 1.0),
        glm::vec4(-1.0, -1.0, -1.0, 1.0),
        glm::vec4(1.0, -1.0, -1.0, 1.0),
        glm::vec4(1.0, 1.0, -1.0, 1.0),
        glm::vec4(-1.0, 1.0, -1.0, 1.0),
    };

    for(int i = 0; i < sizeof(cube_vertices); ++i){
        mesh->vertices.push_back(cube_vertices[i]);
    }

    mesh->set_attr_v_name("coord3d");

    glm::vec4 cube_colors[8] = {
        glm::vec4(1.0, 0.0, 0.0, 1.0),
        glm::vec4(0.0, 1.0, 0.0, 1.0),
        glm::vec4(0.0, 0.0, 1.0, 1.0),
        glm::vec4(1.0, 1.0, 1.0, 1.0),
        glm::vec4(1.0, 0.0, 0.0, 1.0),
        glm::vec4(0.0, 1.0, 0.0, 1.0),
        glm::vec4(0.0, 0.0, 1.0, 1.0),
        glm::vec4(1.0, 1.0, 1.0, 1.0),
    };

    for(int i = 0; i < sizeof(cube_colors); ++i){
        mesh->colors.push_back(cube_colors[i]);
    }

    mesh->set_attr_c_name("v_color");

    GLushort cube_elements[] = {
        // front
        0, 1, 2,
        2, 3, 0,
        // top
        3, 2, 6,
        6, 7, 3,
        // back
        7, 6, 5,
        5, 4, 7,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // left
        4, 0, 3,
        3, 7, 4,
        // right
        1, 5, 6,
        6, 2, 1,
    };

    for(int i = 0; i < sizeof(cube_elements); ++i){
        mesh->elements.push_back(cube_elements[i]);
    }

    mesh->upload();

    render = new Render(mesh, program);

    return 0;
}

void view5_cube_freeResources()
{
    glDisable(GL_BLEND);
    delete program;
    delete mesh;
    delete render;
}

void view5_entry(Window *window)
{
    window->display = view5CubeDisplay;
    window->entry = viewEntry;
    window->init = view5_cube_initResources;
    window->free = view5_cube_freeResources;
}

#ifdef TEST_ALONE
int main(int argc, char *argv[])
{
    Window window;
    resetWindow(&window);

    view5_entry(&window);

    if (mini_initWindow(argc, argv, &window) == 0){
        glutMainLoop();
    }

    return 0;
}
#endif /* TEST_ALONE */
