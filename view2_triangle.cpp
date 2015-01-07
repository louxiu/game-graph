#include "view2_triangle.h"

#include "Mesh.h"
#include "util.h"

Mesh view2_triangle_mesh;
GLint view2_attr_coord2d = -1;
GLuint view2_triangle_program = 0;

int view2_triangle_initResources()
{
    // TODO: this?
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec4 triangle_vertices[3] = {
        glm::vec4(0.0, 0.8, 0.0, 1.0),
        glm::vec4(-0.8, -0.8, 0.0, 1.0),
        glm::vec4(0.8, -0.8, 0.0, 1.0)
    };

    for(int i = 0; i < sizeof(triangle_vertices); ++i){
        view2_triangle_mesh.vertices.push_back(triangle_vertices[i]);
    }

    view2_triangle_mesh.upload();

    view2_triangle_program = create_program("glsl/triangle.2.v.glsl",
                                            "glsl/triangle.2.f.glsl");
    view2_attr_coord2d = get_attrib(view2_triangle_program, "coord2d");
}

void view2TriangleDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(view2_triangle_program);

    view2_triangle_mesh.draw(view2_triangle_mesh,
                             view2_attr_coord2d,
                             -1, -1, -1, -1, -1, -1);

    glutSwapBuffers();
}

void view2_triangle_freeResources()
{
    glDeleteProgram(view2_triangle_program);
}

// TODO: add boost as feature, auto free?

void view2_entry(Window *window)
{
    window->program = view2_triangle_program;
    window->display = view2TriangleDisplay;
    window->entry = viewEntry;
    window->init = view2_triangle_initResources;
    window->free = view2_triangle_freeResources;
}
