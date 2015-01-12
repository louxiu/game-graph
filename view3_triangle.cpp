#include "view3_triangle.h"

#include "Mesh.h"
#include "util.h"

Mesh view3_triangle_mesh;
GLint view3_attr_coord2d = -1,
    view3_attr_color = -1,
    view3_triangle_uniform_fade = -1;

GLuint view3_triangle_program = 0;

void view3TriangleDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(view3_triangle_program);

    view3_triangle_mesh.draw(view3_triangle_mesh,
                             view3_attr_coord2d,
                             view3_attr_color,
                             -1, -1, -1, -1, -1);

    float cur_fade = sinf(glutGet(GLUT_ELAPSED_TIME) /
                          1000.0 * (2 * M_PI) / 5) / 2 + 0.5;

    glUniform1f(view3_triangle_uniform_fade, cur_fade);

    glutSwapBuffers();
}

// TODO: pass vec4 to shader vec3
int view3_triangle_initResources()
{
    glClearColor(1.0, 1.0, 1.0, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    view3_triangle_program = create_program("glsl/triangle.3.v.glsl",
                                            "glsl/triangle.3.f.glsl");

    view3_attr_coord2d = get_attrib(view3_triangle_program, "coord2d");
    view3_attr_color = get_attrib(view3_triangle_program, "v_color");

    const char* uniform_name = "fade";
    view3_triangle_uniform_fade =
        get_uniform(view3_triangle_program, uniform_name);

    glm::vec4 triangle_vertices[3] = {
        glm::vec4(0.0, 0.8, 0.0, 1.0),
        glm::vec4(-0.8, -0.8, 0.0, 1.0),
        glm::vec4(0.8, -0.8, 0.0, 1.0)
    };

    view3_triangle_mesh.vertices.push_back(triangle_vertices[0]);
    view3_triangle_mesh.vertices.push_back(triangle_vertices[1]);
    view3_triangle_mesh.vertices.push_back(triangle_vertices[2]);


    glm::vec4 triangle_colors[3] = {
        glm::vec4(1.0, 1.0, 0.0, 1.0),
        glm::vec4(0.0, 0.0, 1.0, 1.0),
        glm::vec4(1.0, 0.0, 0.0, 1.0)
    };

    for(int i = 0; i < sizeof(triangle_vertices); ++i){
        view3_triangle_mesh.colors.push_back(triangle_colors[i]);
    }

    view3_triangle_mesh.upload();

    return 0;
}

void view3_triangle_freeResources()
{
    glDisable(GL_BLEND);
    glDeleteProgram(view3_triangle_program);
}

void view3_entry(Window *window)
{
    window->program = view3_triangle_program;
    window->display = view3TriangleDisplay;
    window->entry = viewEntry;
    window->init = view3_triangle_initResources;
    window->free = view3_triangle_freeResources;
}
