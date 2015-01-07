#include "view14.h"
#include "util.h"

GLuint view14_program = 0;

GLint view14_attr_coord2d = -1, view14_uniform_vertex_transform = -1;
GLint view14_uniform_texture_transform = 1, view14_uniform_mytexture = -1;
GLuint view14_texture_id = 0;

float view14_offset_x = 0.0, view14_offset_y = 0.0, view14_scale = 1.0;
bool view14_interpolate = false, view14_clamp = false, view14_rotate = false;
GLuint view14_vbo[2];

int view14_initResources()
{
	view14_program =
        create_program("glsl/graph.14.v.glsl", "glsl/graph.14.f.glsl");

	view14_attr_coord2d = get_attrib(view14_program, "coord2d");
	view14_uniform_vertex_transform = get_uniform(view14_program,
                                                  "vertex_transform");
	view14_uniform_texture_transform = get_uniform(view14_program,
                                                   "texture_transform");
	view14_uniform_mytexture = get_uniform(view14_program, "mytexture");

	if (view14_attr_coord2d == -1 || view14_uniform_vertex_transform == -1
        || view14_uniform_texture_transform == -1
        || view14_uniform_mytexture == -1) {
		return 1;
    }

	// Create our datapoints, store it as bytes
#define N 256
	GLbyte graph[N][N];

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			float x = (i - N / 2) / (N / 2.0);
			float y = (j - N / 2) / (N / 2.0);
			float d = hypotf(x, y) * 4.0;
			float z = (1 - d * d) * expf(d * d / -2.0);

			graph[i][j] = roundf(z * 127 + 128);
		}
	}

	/* Upload the texture with our datapoints */
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &view14_texture_id);
	glBindTexture(GL_TEXTURE_2D, view14_texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, N, N, 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, graph);

	// Create two vertex buffer objects
	glGenBuffers(2, view14_vbo);

	// Create an array for 101 * 101 vertices
	glm::vec2 vertices[101][101];

	for (int i = 0; i < 101; i++) {
		for (int j = 0; j < 101; j++) {
			vertices[i][j].x = (j - 50) / 50.0;
			vertices[i][j].y = (i - 50) / 50.0;
		}
	}

	// Tell OpenGL to copy our array to the buffer objects
	glBindBuffer(GL_ARRAY_BUFFER, view14_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create an array of indices into the vertex array that
	// traces both horizontal and vertical lines
	GLushort indices[100 * 101 * 4];
	int i = 0;

	for (int y = 0; y < 101; y++) {
		for (int x = 0; x < 100; x++) {
			indices[i++] = y * 101 + x;
			indices[i++] = y * 101 + x + 1;
		}
	}

	for (int x = 0; x < 101; x++) {
		for (int y = 0; y < 100; y++) {
			indices[i++] = y * 101 + x;
			indices[i++] = (y + 1) * 101 + x;
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, view14_vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
                 indices, GL_STATIC_DRAW);

	return 0;
}

void view14Display()
{
	glUseProgram(view14_program);
	glUniform1i(view14_uniform_mytexture, 0);

	glm::mat4 model;

	if (view14_rotate) {
		model = glm::rotate(glm::mat4(1.0f),
                            glm::radians(glutGet(GLUT_ELAPSED_TIME) / 100.0f),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    } else {
		model = glm::mat4(1.0f);
    }

	glm::mat4 view = glm::lookAt(glm::vec3(0.0, -2.0, 2.0),
                                 glm::vec3(0.0, 0.0, 0.0),
                                 glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 projection = glm::perspective(45.0f, 1.0f * 640 / 480, 0.1f, 10.0f);

	glm::mat4 vertex_transform = projection * view * model;
	glm::mat4 texture_transform =
        glm::translate(glm::scale(glm::mat4(1.0f),
                                  glm::vec3(view14_scale, view14_scale, 1)),
                       glm::vec3(view14_offset_x, view14_offset_y, 0));

	glUniformMatrix4fv(view14_uniform_vertex_transform, 1, GL_FALSE,
                       glm::value_ptr(vertex_transform));
	glUniformMatrix4fv(view14_uniform_texture_transform, 1, GL_FALSE,
                       glm::value_ptr(texture_transform));

	glClear(GL_COLOR_BUFFER_BIT);

	/* Set texture wrapping mode */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    view14_clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    view14_clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);

	/* Set texture interpolation mode */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    view14_interpolate ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    view14_interpolate ? GL_LINEAR : GL_NEAREST);

	/* Draw the grid using the indices to our
     * vertices using our vertex buffer objects */

	glEnableVertexAttribArray(view14_attr_coord2d);

	glBindBuffer(GL_ARRAY_BUFFER, view14_vbo[0]);
	glVertexAttribPointer(view14_attr_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, view14_vbo[1]);
	glDrawElements(GL_LINES, 100 * 101 * 4, GL_UNSIGNED_SHORT, 0);

	/* Stop using the vertex buffer object */
	glDisableVertexAttribArray(view14_attr_coord2d);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glutSwapBuffers();
}

// TODO: merge pengium warrior to this
void view14_special(int key, int x, int y)
{
	switch (key) {
        case GLUT_KEY_F1:
            view14_interpolate = !view14_interpolate;
            // printf("Interpolation is now %s\n",
            //        view14_interpolate ? "on" : "off");
            break;
        case GLUT_KEY_F2:
            view14_clamp = !view14_clamp;
            // printf("Clamping is now %s\n", view14_clamp ? "on" : "off");
            break;
        case GLUT_KEY_F3:
            view14_rotate = !view14_rotate;
            // printf("Rotation is now %s\n", view14_rotate ? "on" : "off");
            break;
        case GLUT_KEY_LEFT:
            view14_offset_x -= 0.03;
            break;
        case GLUT_KEY_RIGHT:
            view14_offset_x += 0.03;
            break;
        case GLUT_KEY_UP:
            view14_offset_y += 0.03;
            break;
        case GLUT_KEY_DOWN:
            view14_offset_y -= 0.03;
            break;
        case GLUT_KEY_PAGE_UP:
            view14_scale *= 1.5;
            break;
        case GLUT_KEY_PAGE_DOWN:
            view14_scale /= 1.5;
            break;
        case GLUT_KEY_HOME:
            view14_offset_x = 0.0;
            view14_offset_y = 0.0;
            view14_scale = 1.0;
            break;
	}

	glutPostRedisplay();
}

void view14_freeResources()
{
    glDeleteProgram(view14_program);
    glDeleteBuffers(2, view14_vbo);
    glDeleteTextures(1, &view14_texture_id);
}

void view14_entry(Window *window)
{
	window->program = view14_program;
    window->display = view14Display;
    window->entry = viewEntry;
    window->init = view14_initResources;
    window->free = view14_freeResources;
    window->special = view14_special;
}
