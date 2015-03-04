#include <iostream>
#include <vector>
using namespace std;

#include "view23_pcd.h"
#include "util.h"

GLuint view23_program = 0;
GLuint view23_vbo = 0;
GLint view23_attr_coord2d;

void view23_pcdDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(view23_program);

    glBindBuffer(GL_ARRAY_BUFFER, view23_vbo);
    glEnableVertexAttribArray(view23_attr_coord2d);
	glVertexAttribPointer(view23_attr_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
	// switch (view23_mode) {}
	glutSwapBuffers();
}

int view23_pcd_initResources()
{
    glClearColor(1.0, 1.0, 1.0, 0);
	view23_program = create_program("glsl/pcd.23.v.glsl",
                                    "glsl/pcd.23.f.glsl");

    view23_attr_coord2d = get_attrib(view23_program, "coord2d");

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create the vertex buffer object
	glGenBuffers(1, &view23_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, view23_vbo);

	// Create our own temporary buffer
	point graph[2000];

	// Fill it in just like an array
	for (int i = 0; i < 2000; i++) {
		float x = (i - 1000.0) / 100.0;

		graph[i].x = x;
		graph[i].y = sin(x * 10.0) / (1.0 + x * x);
	}

	// Tell OpenGL to copy our array to the buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof(graph), graph, GL_STATIC_DRAW);
}

void view23_pcd_freeResources()
{
	glDisable(GL_BLEND);

    glDeleteProgram(view23_program);
    glDeleteBuffers(1, &view23_vbo);
}

void view23_special(int key, int x, int y)
{

}
// TODO: use mipmap to optimize
// TODO: GPU?
void view23_pcd_entry(Window *window)
{
    window->display = view23_pcdDisplay;
    window->entry = viewEntry;
    window->special = view23_special;
    window->init = view23_pcd_initResources;
    window->free = view23_pcd_freeResources;
}

#ifdef TEST_ALONE
int main(int argc, char *argv[])
{
    Window window;
    resetWindow(&window);

    view23_pcd_entry(&window);

    if (mini_initWindow(argc, argv, &window) == 0){
        glutMainLoop();
    }

    return 0;
}
#endif /* TEST_ALONE */

