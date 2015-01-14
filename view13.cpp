#include "view13.h"
#include "util.h"

GLuint view13_program = 0;
GLint view13_attr_coord2d = -1, view13_uniform_color = -1;
GLint view13_uniform_transform = -1;

float view13_offset_x = 0, view13_scale_x = 1;
GLuint view13_vbo[3] = {0};

const int view13_border = 10, view13_ticksize = 10;

int view13_initResources()
{
    glClearColor(1.0, 1.0, 1.0, 0);

	view13_program = create_program("glsl/graph.13.v.glsl",
                                    "glsl/graph.13.f.glsl");
	view13_attr_coord2d = get_attrib(view13_program, "coord2d");
	view13_uniform_transform = get_uniform(view13_program, "transform");
	view13_uniform_color = get_uniform(view13_program, "color");

	// Create the vertex buffer object
	glGenBuffers(3, view13_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, view13_vbo[0]);

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

	// Create a VBO for the border
	static const point border[4] = { {-1, -1}, {1, -1}, {1, 1}, {-1, 1} };
	glBindBuffer(GL_ARRAY_BUFFER, view13_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(border), border, GL_STATIC_DRAW);

	return 0;
}

// TODO: the func of view13_viewport_transform
glm::mat4 view13_viewport_transform(float x, float y, float width, float height,
                                    float *pixel_x = 0, float *pixel_y = 0)
{
	// Map OpenGL coordinates (-1,-1) to window coordinates (x,y),
	// (1,1) to (x + width, y + height).

	// First, we need to know the real window size:
	float window_width = glutGet(GLUT_WINDOW_WIDTH);
	float window_height = glutGet(GLUT_WINDOW_HEIGHT);

	// Calculate how to translate the x and y coordinates:
	float offset_x = (2.0 * x + (width - window_width)) / window_width;
	float offset_y = (2.0 * y + (height - window_height)) / window_height;

	// Calculate how to rescale the x and y coordinates:
	float scale_x = width / window_width;
	float scale_y = height / window_height;

	// Calculate size of pixels in OpenGL coordinates
	if (pixel_x) {
		*pixel_x = 2.0 / width;
    }

    if (pixel_y) {
		*pixel_y = 2.0 / height;
    }

	return glm::scale(glm::translate(glm::mat4(1),
                                     glm::vec3(offset_x, offset_y, 0)),
                      glm::vec3(scale_x, scale_y, 1));
}

void view13Display()
{
    // TODO: this is subwindow size? modify other
	int window_width = glutGet(GLUT_WINDOW_WIDTH);
	int window_height = glutGet(GLUT_WINDOW_HEIGHT);

	glUseProgram(view13_program);

	glClear(GL_COLOR_BUFFER_BIT);

	/* ---------------------------------------------------------------- */
	/* Draw the graph */

	// Set our viewport, this will clip geometry
	glViewport(view13_border + view13_ticksize, view13_border + view13_ticksize,
               window_width - view13_border * 2 - view13_ticksize,
               window_height - view13_border * 2 - view13_ticksize);

	// Set the scissor rectangle,this will clip fragments
	glScissor(view13_border + view13_ticksize, view13_border + view13_ticksize,
              window_width - view13_border * 2 - view13_ticksize,
              window_height - view13_border * 2 - view13_ticksize);

	glEnable(GL_SCISSOR_TEST);

	// Set our coordinate transformation matrix
	glm::mat4 transform =
        glm::translate(glm::scale(glm::mat4(1.0f),
                                  glm::vec3(view13_scale_x, 1, 1)),
                       glm::vec3(view13_offset_x, 0, 0));

    glUniformMatrix4fv(view13_uniform_transform, 1, GL_FALSE,
                       glm::value_ptr(transform));

	// Set the color to red
	GLfloat red[4] = { 1, 0, 0, 1 };
	glUniform4fv(view13_uniform_color, 1, red);

	// Draw using the vertices in our vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, view13_vbo[0]);

	glEnableVertexAttribArray(view13_attr_coord2d);
	glVertexAttribPointer(view13_attr_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINE_STRIP, 0, 2000);

	// Stop clipping
	glViewport(0, 0, window_width, window_height);
	glDisable(GL_SCISSOR_TEST);

	/* ---------------------------------------------------------------- */
	/* Draw the borders */

	float pixel_x, pixel_y;

	// Calculate a transformation matrix that gives us the same normalized device
	// coordinates as above

	transform = view13_viewport_transform(view13_border + view13_ticksize,
                                          view13_border + view13_ticksize,
                                          window_width - view13_border * 2 - view13_ticksize,
                                          window_height - view13_border * 2 - view13_ticksize,
                                          &pixel_x, &pixel_y);

	// Tell our vertex shader about it
	glUniformMatrix4fv(view13_uniform_transform, 1,
                       GL_FALSE, glm::value_ptr(transform));

	// Set the color to black
	GLfloat black[4] = {0, 0, 0, 1};
	glUniform4fv(view13_uniform_color, 1, black);

	// Draw a border around our graph
	glBindBuffer(GL_ARRAY_BUFFER, view13_vbo[1]);
	glVertexAttribPointer(view13_attr_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

	/* ---------------------------------------------------------------- */
	/* Draw the y tick marks */

	point ticks[42];

	for (int i = 0; i <= 20; i++) {
		float y = -1 + i * 0.1;
		float tickscale = (i % 10) ? 0.5 : 1;

		ticks[i * 2].x = -1;
		ticks[i * 2].y = y;
		ticks[i * 2 + 1].x = -1 - view13_ticksize * tickscale * pixel_x;
		ticks[i * 2 + 1].y = y;
	}

	glBindBuffer(GL_ARRAY_BUFFER, view13_vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ticks), ticks, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(view13_attr_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 42);

	/* ---------------------------------------------------------------- */
	/* Draw the x tick marks */

    // desired space between ticks, in graph coordinates
	float tickspacing = 0.1 * powf(10, -floor(log10(view13_scale_x)));

    // left edge, in graph coordinates
	float left = -1.0 / view13_scale_x - view13_offset_x;

    // right edge, in graph coordinates
	float right = 1.0 / view13_scale_x - view13_offset_x;

    // index of left tick, counted from the origin
	int left_i = ceil(left / tickspacing);

    // index of right tick, counted from the origin
    int right_i = floor(right / tickspacing);

    // space between left edge of graph and the first tick
	float rem = left_i * tickspacing - left;

	// first tick in device coordinates
	float firsttick = -1.0 + rem * view13_scale_x;

	int nticks = right_i - left_i + 1;	// number of ticks to show

	if (nticks > 21)
		nticks = 21;	// should not happen

	for (int i = 0; i < nticks; i++) {
		float x = firsttick + i * tickspacing * view13_scale_x;
		float tickscale = ((i + left_i) % 10) ? 0.5 : 1;

		ticks[i * 2].x = x;
		ticks[i * 2].y = -1;
		ticks[i * 2 + 1].x = x;
		ticks[i * 2 + 1].y = -1 - view13_ticksize * tickscale * pixel_y;
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(ticks), ticks, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(view13_attr_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, nticks * 2);

	// And we are done.

	glDisableVertexAttribArray(view13_attr_coord2d);
	glutSwapBuffers();
}

void view13_special(int key, int x, int y)
{
	switch (key) {
        case GLUT_KEY_LEFT:
            view13_offset_x -= 0.03;
            break;
        case GLUT_KEY_RIGHT:
            view13_offset_x += 0.03;
            break;
        case GLUT_KEY_UP:
            view13_scale_x *= 1.5;
            break;
        case GLUT_KEY_DOWN:
            view13_scale_x /= 1.5;
            break;
        case GLUT_KEY_HOME:
            view13_offset_x = 0.0;
            view13_scale_x = 1.0;
            break;
	}

	glutPostRedisplay();
}

void view13_freeResources()
{
	glDisable(GL_SCISSOR_TEST);
    glDeleteProgram(view13_program);
}

void view13_entry(Window *window)
{
    window->program = view13_program;
    window->display = view13Display;
    window->entry = viewEntry;
    window->init = view13_initResources;
    window->free = view13_freeResources;
    window->special = view13_special;
}

#ifdef TEST_ALONE
int main(int argc, char *argv[])
{
    Window window;
    resetWindow(&window);

    view13_entry(&window);

    if (mini_initWindow(argc, argv, &window) == 0){
        glutMainLoop();
    }

    return 0;
}
#endif /* TEST_ALONE */
