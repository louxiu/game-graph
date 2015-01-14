#include "view17_cube.h"
#include "util.h"

GLuint view17_cube_program = 0;

GLuint view17_vbo_cube_vertices = 0, view17_vbo_cube_colors = 0,
    view17_ibo_cube_elements = 0;

GLint view17_attribute_coord3d = -1, view17_attribute_v_color = -1,
    view17_attribute_coord2d = -1;

GLint view17_uniform_mvp = -1, view17_uniform_p = 1;

GLuint view17_program_stencil = 0;

int view17_screen_width = SUB_WINDOW_WIDTH,
    view17_screen_height = SUB_WINDOW_HEIGHT;

int view17_initResources()
{
    glEnable(GL_DEPTH_TEST);

    glClearColor(1.0, 1.0, 1.0, 1.0);

    GLfloat cube_vertices[] = {
        // front
        -1.0, -1.0,  1.0,
        1.0, -1.0,  1.0,
        1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        // back
        -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0,
        1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0,
    };
    glGenBuffers(1, &view17_vbo_cube_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, view17_vbo_cube_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices),
                 cube_vertices, GL_STATIC_DRAW);

    GLfloat cube_colors[] = {
        // front colors
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
        1.0, 1.0, 1.0,
        // back colors
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
        1.0, 1.0, 1.0,
    };
    glGenBuffers(1, &view17_vbo_cube_colors);
    glBindBuffer(GL_ARRAY_BUFFER, view17_vbo_cube_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors),
                 cube_colors, GL_STATIC_DRAW);

    GLushort cube_elements[] = {
        // front
        0, 1, 2,
        2, 3, 0,
        // top
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // bottom
        4, 0, 3,
        3, 7, 4,
        // left
        4, 5, 1,
        1, 0, 4,
        // right
        3, 2, 6,
        6, 7, 3,
    };
    glGenBuffers(1, &view17_ibo_cube_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, view17_ibo_cube_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements),
                 cube_elements, GL_STATIC_DRAW);

    view17_cube_program = create_program("glsl/cube.17.v.glsl",
                                           "glsl/cube.17.f.glsl");

    const char* attribute_name;
    attribute_name = "coord3d";
    view17_attribute_coord3d = get_attrib(view17_cube_program,
                                          attribute_name);
    attribute_name = "v_color";
    view17_attribute_v_color = get_attrib(view17_cube_program,
                                          attribute_name);

    const char* uniform_name;
    uniform_name = "mvp";
    view17_uniform_mvp = glGetUniformLocation(view17_cube_program,
                                              uniform_name);


    view17_program_stencil = create_program("glsl/stencil.17.v.glsl",
                                            "glsl/stencil.17.f.glsl");
    attribute_name = "coord2d";
    view17_attribute_coord2d = get_attrib(view17_program_stencil,
                                          attribute_name);
    uniform_name = "p";
    view17_uniform_p = glGetUniformLocation(view17_program_stencil,
                                            uniform_name);
    return 0;
}

void view17_draw_circle()
{
#define RES 40
#define RADIUS .5
    GLfloat vertices[(RES+1)*2];

    // -.5<->+.5 every 8 seconds
    float center_x = sinf(glutGet(GLUT_ELAPSED_TIME)
                          / 1000.0 * (2*3.14) / 8) * .5;

    // -.5<->+.5 every 4 seconds
    float center_y = sinf(glutGet(GLUT_ELAPSED_TIME)
                          / 1000.0 * (2*3.14) / 4) * .5;

    // circle center
    vertices[(RES)*2]   = center_x;
    vertices[(RES)*2+1] = center_y;

    // circle points
    for (int i = 0; i < RES; i++) {
        float angle = 2 * M_PI * i / RES;
        GLfloat x = center_x + cos(angle) * RADIUS;
        GLfloat y = center_y + sin(angle) * RADIUS;
        vertices[i*2]   = x;
        vertices[i*2+1] = y;
    }

    // circle elements
    GLushort elements[(RES)*3];
    for (int i = 0; i < RES; i++) {
        elements[i*3]   = RES;
        elements[i*3+1] = i;
        elements[i*3+2] = (i+1)%RES;
    }

    GLuint vbo_vertices;
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint ibo_elements;
    glGenBuffers(1, &ibo_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements),
                 elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // projection matrix to deal with aspect ratio
    glUseProgram(view17_program_stencil);
    float aspect = 1.0 * view17_screen_width / view17_screen_height;
    glm::mat4 projection = glm::ortho(-1.0f * aspect, 1.0f * aspect,
                                      -1.0f, 1.0f);
    glUniformMatrix4fv(view17_uniform_p, 1, GL_FALSE,
                       glm::value_ptr(projection));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glEnableVertexAttribArray(view17_attribute_coord2d);
    glVertexAttribPointer(
        view17_attribute_coord2d,  // attribute
        2,                  // number of elements per vertex, here (x,y,z,w)
        GL_FLOAT,           // the type of each element
        GL_FALSE,           // take our values as-is
        0,                  // no extra data between each position
        0                   // offset of first element
                          );

    glDrawElements(GL_TRIANGLES, sizeof(elements)/sizeof(elements[0]),
                   GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(view17_attribute_coord2d);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &vbo_vertices);
    glDeleteBuffers(1, &ibo_elements);
}

void view17_draw_scene()
{
    glUseProgram(view17_cube_program);
    glEnableVertexAttribArray(view17_attribute_coord3d);
    // Describe our vertices array to OpenGL
    // (it can't guess its format automatically)
    glBindBuffer(GL_ARRAY_BUFFER, view17_vbo_cube_vertices);
    glVertexAttribPointer(
        view17_attribute_coord3d, // attribute
        3,                 // number of elements per vertex, here (x,y,z)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                  // offset of first element
                          );

    glEnableVertexAttribArray(view17_attribute_v_color);
    glBindBuffer(GL_ARRAY_BUFFER, view17_vbo_cube_colors);
    glVertexAttribPointer(
        view17_attribute_v_color, // attribute
        3,                 // number of elements per vertex, here (R,G,B)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                  // offset of first element
                          );

    /* Push each element in buffer_vertices to the vertex shader */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, view17_ibo_cube_elements);
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(view17_attribute_coord3d);
    glDisableVertexAttribArray(view17_attribute_v_color);
}

void view17Display()
{
    // TODO: how to create shadow volume
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    glStencilFunc(GL_NEVER, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);  // draw 1s on test fail (always)

    // clear the stencil buffer with zero
    glStencilMask(0xFF);
    glClear(GL_STENCIL_BUFFER_BIT);  // needs mask=0xFF

    // draw the circle in stencil buffer filled with 1 in it
    view17_draw_circle();

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glStencilMask(0x00);

    // revert
    // glStencilFunc(GL_EQUAL, 0, 0xFF);
    glStencilFunc(GL_EQUAL, 1, 0xFF);

    view17_draw_scene();

    glDisable(GL_STENCIL_TEST);

    glutSwapBuffers();
}

void view17Idle()
{
    // 30° per second
    float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 30;
    glm::vec3 axis_y(0, 1, 0);
    glm::mat4 anim = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_y);

    glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                     glm::vec3(0.0, 0.0, -4.0));
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0),
                                 glm::vec3(0.0, 0.0, -4.0),
                                 glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 projection =
        glm::perspective(45.0f,
                         1.0f * view17_screen_width / view17_screen_height,
                         0.1f, 10.0f);

    glm::mat4 mvp = projection * view * model * anim;

    glUseProgram(view17_cube_program);
    glUniformMatrix4fv(view17_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
    glutPostRedisplay();
}

void view17Reshape(int width, int height)
{
    view17_screen_width = width;
    view17_screen_height = height;
    glViewport(0, 0, view17_screen_width, view17_screen_height);
}

void view17_freeResources()
{
    glDisable(GL_STENCIL_TEST);

    glDeleteProgram(view17_cube_program);
    glDeleteBuffers(1, &view17_vbo_cube_vertices);
    glDeleteBuffers(1, &view17_vbo_cube_colors);
    glDeleteBuffers(1, &view17_ibo_cube_elements);
}

void view17_entry(Window *window)
{
    window->program = view17_cube_program;
    window->display = view17Display;
    window->entry = viewEntry;
    window->init = view17_initResources;
    window->free = view17_freeResources;
    window->idle = view17Idle;
    window->reshape = view17Reshape;
}

#ifdef TEST_ALONE
int main(int argc, char *argv[])
{
    Window window;
    resetWindow(&window);

    view17_entry(&window);

    if (mini_initWindow(argc, argv, &window) == 0){
        glutMainLoop();
    }

    return 0;
}
#endif /* TEST_ALONE */
