#include "view4_triangle.h"

#include "Mesh.h"
#include "util.h"

Mesh view4_triangle_mesh;
GLint view4_attr_coord2d = -1,
    view4_uniform_m_transform = -1;

GLuint view4_triangle_program = 0;

void view4TriangleDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(view4_triangle_program);

    float move = sinf(glutGet(GLUT_ELAPSED_TIME) / 1000.0 * (2*3.14) / 5);
    float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 45;
    glm::vec3 axis_z(0, 0, 1);
    glm::mat4 m_transform =
        glm::translate(glm::mat4(1.0f), glm::vec3(move, 0.0, 0.0)) *
        glm::rotate(glm::mat4(1.0f), angle, axis_z);

    glUniformMatrix4fv(view4_uniform_m_transform, 1, GL_FALSE,
                       glm::value_ptr(m_transform));

    view4_triangle_mesh.draw(view4_triangle_mesh,
                             view4_attr_coord2d,
                             -1, -1, -1, -1, -1, -1);

    glutSwapBuffers();
}

int view4_triangle_initResources()
{
    glClearColor(1.0, 1.0, 1.0, 0);

    view4_triangle_program = create_program("glsl/triangle.4.v.glsl",
                                          "glsl/triangle.4.f.glsl");
    view4_attr_coord2d = get_attrib(view4_triangle_program, "coord2d");

    const char *uniform_name = "m_transform";
    view4_uniform_m_transform = get_uniform(view4_triangle_program,
                                            uniform_name);

    if (view4_uniform_m_transform == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
        return 1;
    }

    glm::vec4 triangle_vertices[3] = {
        glm::vec4(0.0, 0.8, 0.0, 1.0),
        glm::vec4(-0.8, -0.8, 0.0, 1.0),
        glm::vec4(0.8, -0.8, 0.0, 1.0)
    };

    for(int i = 0; i < sizeof(triangle_vertices); ++i){
        view4_triangle_mesh.vertices.push_back(triangle_vertices[i]);
    }

    view4_triangle_mesh.upload();

    return 0;
}

void view4_triangle_freeResources()
{

    glDeleteProgram(view4_triangle_program);
}

void view4_entry(Window *window)
{
    window->program = view4_triangle_program;
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
