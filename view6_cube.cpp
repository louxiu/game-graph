#include <iostream>
#include <vector>
using namespace std;

#include "view6_cube.h"
#include "util.h"

#include "engine/Texture.h"
#include "engine/Mesh.h"
#include "engine/Program.h"
#include "engine/Render.h"

static Program *program = NULL;
static Texture *texture = NULL;
static Mesh *mesh = NULL;
static Render *render = NULL;

void view6CubeDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // animate
    float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * glm::radians(15.0);
    glm::mat4 anim = \
        glm::rotate(glm::mat4(1.0f), angle*3.0f, glm::vec3(1, 0, 0)) *
        glm::rotate(glm::mat4(1.0f), angle*2.0f, glm::vec3(0, 1, 0)) *
        glm::rotate(glm::mat4(1.0f), angle*4.0f, glm::vec3(0, 0, 1));

    glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                     glm::vec3(0.0, 0.0, -4.0));
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0),
                                 glm::vec3(0.0, 0.0, -4.0),
                                 glm::vec3(0.0, 1.0, 0.0));

    glm::mat4 projection = glm::perspective(45.0f,
                                            1.0f*128/128,
                                            0.1f, 10.0f);

    glm::mat4 mvp = projection * view * model * anim;

    render->begin();
    program->set_uniformMatrix4fv("mvp", 1, GL_FALSE,
                                  glm::value_ptr(mvp));
    program->set_uniform1i("mytexture", texture->getTbo());
    render->draw();
    render->end();

    glutSwapBuffers();
}

int view6_cube_initResources()
{
    glClearColor(1.0, 1.0, 1.0, 0);

    glEnable(GL_DEPTH_TEST);

    mesh = new Mesh();

    glm::vec4 cube_vertices[] = {
        // front
        glm::vec4(-1.0, -1.0,  1.0, 1.0),
        glm::vec4(1.0, -1.0,  1.0, 1.0),
        glm::vec4(1.0,  1.0,  1.0, 1.0),
        glm::vec4(-1.0,  1.0,  1.0, 1.0),
        // top
        glm::vec4(-1.0,  1.0,  1.0, 1.0),
        glm::vec4(1.0,  1.0,  1.0, 1.0),
        glm::vec4(1.0,  1.0, -1.0, 1.0),
        glm::vec4(-1.0,  1.0, -1.0, 1.0),
        // back
        glm::vec4(1.0, -1.0, -1.0, 1.0),
        glm::vec4(-1.0, -1.0, -1.0, 1.0),
        glm::vec4(-1.0,  1.0, -1.0, 1.0),
        glm::vec4(1.0,  1.0, -1.0, 1.0),
        // bottom
        glm::vec4(-1.0, -1.0, -1.0, 1.0),
        glm::vec4(1.0, -1.0, -1.0, 1.0),
        glm::vec4(1.0, -1.0,  1.0, 1.0),
        glm::vec4(-1.0, -1.0,  1.0, 1.0),
        // left
        glm::vec4(-1.0, -1.0, -1.0, 1.0),
        glm::vec4(-1.0, -1.0,  1.0, 1.0),
        glm::vec4(-1.0,  1.0,  1.0, 1.0),
        glm::vec4(-1.0,  1.0, -1.0, 1.0),
        // right
        glm::vec4(1.0, -1.0,  1.0, 1.0),
        glm::vec4(1.0, -1.0, -1.0, 1.0),
        glm::vec4(1.0,  1.0, -1.0, 1.0),
        glm::vec4(1.0,  1.0,  1.0, 1.0),
    };

    for(int i = 0; i < sizeof(cube_vertices); ++i){
        mesh->vertices.push_back(cube_vertices[i]);
    }
    mesh->set_attr_v_name("coord3d");

    GLushort cube_elements[] = {
        // front
        0,  1,  2,
        2,  3,  0,
        // top
        4,  5,  6,
        6,  7,  4,
        // back
        8,  9, 10,
        10, 11,  8,
        // bottom
        12, 13, 14,
        14, 15, 12,
        // left
        16, 17, 18,
        18, 19, 16,
        // right
        20, 21, 22,
        22, 23, 20,
    };

    for(int i = 0; i < sizeof(cube_elements); ++i){
        mesh->elements.push_back(cube_elements[i]);
    }

    glm::vec4 cube_texture_vertices[4] = {
        // front
        glm::vec4(0.0, 0.0, 0.0, 0.0),
        glm::vec4(1.0, 0.0, 0.0, 0.0),
        glm::vec4(1.0, 1.0, 0.0, 0.0),
        glm::vec4(0.0, 1.0, 0.0, 0.0),
    };

    for(int i = 0; i < 4 * 6; ++i){
        mesh->texture_vertices.push_back(cube_texture_vertices[i%4]);
    }

    mesh->set_attr_tv_name("texcoord");
    mesh->upload();

    program = new Program("glsl/cube.6.v.glsl",
                          "glsl/cube.6.f.glsl");

    texture = new Texture("data/res_texture.jpg");

    render = new Render(mesh, program);

    return 0;
}

void view6_cube_freeResources()
{
    glDisable(GL_DEPTH_TEST);

    delete program;
    delete mesh;
    delete render;
}

void view6_entry(Window *window)
{
    window->display = view6CubeDisplay;
    window->entry = viewEntry;
    window->init = view6_cube_initResources;
    window->free = view6_cube_freeResources;
}

#ifdef TEST_ALONE
int main(int argc, char *argv[])
{
    Window window;
    resetWindow(&window);

    view6_entry(&window);

    if (mini_initWindow(argc, argv, &window) == 0){
        glutMainLoop();
    }

    return 0;
}
#endif /* TEST_ALONE */
