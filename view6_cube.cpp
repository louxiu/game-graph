#include "view5_cube.h"

#include "Mesh.h"
#include "util.h"

#include <SOIL/SOIL.h>

// TODO: make this as a class
Mesh view6_cube_mesh;
GLint view6_cube_attr_coord3d = -1, view6_cube_attr_texcoord = -1,
    view6_uniform_texture = -1, view6_uniform_mvp = -1;
GLuint view6_cube_program = 0;

void view6CubeDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(view6_cube_program);

    view6_cube_mesh.draw(view6_cube_mesh,
                         view6_cube_attr_coord3d,
                         -1,
                         -1,
                         view6_cube_attr_texcoord,
                         view6_uniform_texture,
                         -1,
                         -1);

    // animate
    float angle = glutGet(GLUT_ELAPSED_TIME) / 20.0 * glm::radians(15.0);
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
    glUniformMatrix4fv(view6_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

    glutSwapBuffers();
}

int view6_cube_initResources()
{
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    view6_cube_program = create_program("glsl/cube.6.v.glsl",
                                          "glsl/cube.6.f.glsl");

    view6_cube_attr_coord3d = get_attrib(view6_cube_program, "coord3d");
    view6_cube_attr_texcoord = get_attrib(view6_cube_program, "texcoord");

    const char *uniform_name = "mytexture";
    view6_uniform_texture = get_uniform(view6_cube_program,
                                        uniform_name);
    uniform_name = "mvp";
    view6_uniform_mvp = get_uniform(view6_cube_program, uniform_name);

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
        view6_cube_mesh.vertices.push_back(cube_vertices[i]);
    }

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
        view6_cube_mesh.elements.push_back(cube_elements[i]);
    }

    glm::vec4 cube_texture_vertices[4] = {
        // front
        glm::vec4(0.0, 0.0, 0.0, 0.0),
        glm::vec4(1.0, 0.0, 0.0, 0.0),
        glm::vec4(1.0, 1.0, 0.0, 0.0),
        glm::vec4(0.0, 1.0, 0.0, 0.0),
    };

    // for (int i = 1; i < 6; i++){
    //     memcpy(&cube_texture_vertices[i * 4], &cube_texture_vertices[0],
    //            4 * sizeof(glm::vec4));
    // }

    // TODO: printf ("j = %d\n", sizeof(cube_texture_vertices));
    for(int i = 0; i < 4 * 6; ++i){
        view6_cube_mesh.texture_vertices.push_back(cube_texture_vertices[i%4]);
    }

    // TODO: transparent problem
    view6_cube_mesh.texture = SOIL_load_image("data/res_texture.jpg",
                                              &view6_cube_mesh.texture_width,
                                              &view6_cube_mesh.texture_height,
                                              NULL, 0);

    view6_cube_mesh.upload();

    return 0;
}

void view6_cube_freeResources()
{
    glDeleteProgram(view6_cube_program);
}
