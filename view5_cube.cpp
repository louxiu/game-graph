#include "view5_cube.h"

#include "Mesh.h"
#include "util.h"

GLuint view5_cube_program = 0;
Mesh view5_cube_mesh;
GLint view5_cube_attr_coord3d = -1,
    view5_cube_attr_color = -1,
    view5_uniform_mvp = -1;

void view5CubeDisplay()
{
    // TODO: the cube transparent problem
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(view5_cube_program);

    // TODO: perspective internal
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0));
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0),
                                 glm::vec3(0.0, 0.0, -4.0),
                                 glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 projection = glm::perspective(45.0f, 1.0f*128/128, 0.1f, 10.0f);

    float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 45;  // 45° per second
    glm::vec3 axis_y(0.0, 1.0, 0.0);
    glm::mat4 anim = glm::rotate(glm::mat4(1.0f), angle, axis_y);

    glm::mat4 mvp = projection * view * model * anim;

    glUniformMatrix4fv(view5_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

    view5_cube_mesh.draw(view5_cube_mesh,
                         view5_cube_attr_coord3d,
                         view5_cube_attr_color,
                         -1, -1, -1, -1, -1);

    glutSwapBuffers();
}

int view5_cube_initResources()
{
    // glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    view5_cube_program = create_program("glsl/cube.5.v.glsl",
                                          "glsl/cube.5.f.glsl");

    view5_cube_attr_coord3d = get_attrib(view5_cube_program, "coord3d");
    view5_cube_attr_color = get_attrib(view5_cube_program, "v_color");

    const char *uniform_name = "mvp";
    view5_uniform_mvp = get_uniform(view5_cube_program, uniform_name);
    if (view5_uniform_mvp == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
        return 1;
    }

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
        view5_cube_mesh.vertices.push_back(cube_vertices[i]);
    }

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
        view5_cube_mesh.colors.push_back(cube_colors[i]);
    }

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
        view5_cube_mesh.elements.push_back(cube_elements[i]);
    }

    view5_cube_mesh.upload();

    return 0;
}

void view5_cube_freeResources()
{
    glDeleteProgram(view5_cube_program);
}
