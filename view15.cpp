#include "view15.h"
#include "util.h"

GLuint view15_program = 0;

GLint view15_attr_coord2d = -1;
GLint view15_uniform_vertex_transform = -1,
    view15_uniform_texture_transform = -1, view15_uniform_color = -1;

GLuint view15_texture_id = 0;

GLint view15_uniform_mytexture = -1;
float view15_offset_x = 0.0, view15_offset_y = 0.0, view15_scale = 1.0;
bool view15_interpolate = false, view15_clamp = false, view15_rotate = false,
    view15_polygonoffset = true;

GLuint view15_vbo[3];

int view15_initResources()
{
	glClearColor(1.0, 1.0, 1.0, 0);

	view15_program = create_program("glsl/graph.15.v.glsl",
									"glsl/graph.15.f.glsl");

	view15_attr_coord2d = get_attrib(view15_program, "coord2d");
	view15_uniform_vertex_transform = get_uniform(view15_program,
                                                  "vertex_transform");
	view15_uniform_texture_transform = get_uniform(view15_program,
                                                   "texture_transform");
	view15_uniform_mytexture = get_uniform(view15_program, "mytexture");
	view15_uniform_color = get_uniform(view15_program, "color");

	if (view15_attr_coord2d == -1 || view15_uniform_vertex_transform == -1
        || view15_uniform_texture_transform == -1
        || view15_uniform_mytexture == -1 || view15_uniform_color == -1) {
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
	glGenTextures(1, &view15_texture_id);
	glBindTexture(GL_TEXTURE_2D, view15_texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, N, N, 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, graph);

	// Create two vertex buffer objects
	glGenBuffers(3, view15_vbo);

	// Create an array for 101 * 101 vertices
	glm::vec2 vertices[101][101];

	for (int i = 0; i < 101; i++) {
		for (int j = 0; j < 101; j++) {
			vertices[i][j].x = (j - 50) / 50.0;
			vertices[i][j].y = (i - 50) / 50.0;
		}
	}

	// Tell OpenGL to copy our array to the buffer objects
	glBindBuffer(GL_ARRAY_BUFFER, view15_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create an array of indices into the vertex array that
	// traces both horizontal and vertical lines
	GLushort indices[100 * 101 * 6];
	int i = 0;

	for (int y = 0; y < 100; y++) {
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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, view15_vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 100 * 101 * 4 * sizeof(*indices),
                 indices, GL_STATIC_DRAW);

	// Create another array of indices that describes all the
	// triangles needed to create a completely filled surface
	i = 0;

	for (int y = 0; y < 101; y++) {
		for (int x = 0; x < 100; x++) {
			indices[i++] = y * 101 + x;
			indices[i++] = y * 101 + x + 1;
			indices[i++] = (y + 1) * 101 + x + 1;

			indices[i++] = y * 101 + x;
			indices[i++] = (y + 1) * 101 + x + 1;
			indices[i++] = (y + 1) * 101 + x;
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, view15_vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

	return 0;
}

void view15Display()
{
	glUseProgram(view15_program);
	glUniform1i(view15_uniform_mytexture, 0);

	glm::mat4 model;

	if (view15_rotate) {
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
	glm::mat4 texture_transform = glm::translate(
        glm::scale(glm::mat4(1.0f),
                   glm::vec3(view15_scale, view15_scale, 1)),
        glm::vec3(view15_offset_x, view15_offset_y, 0));

	glUniformMatrix4fv(view15_uniform_vertex_transform, 1, GL_FALSE,
                       glm::value_ptr(vertex_transform));
	glUniformMatrix4fv(view15_uniform_texture_transform, 1, GL_FALSE,
                       glm::value_ptr(texture_transform));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Set texture wrapping mode */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    view15_clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    view15_clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);

	/* Set texture interpolation mode */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    view15_interpolate ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    view15_interpolate ? GL_LINEAR : GL_NEAREST);

	/* Draw the triangles, a little dark, with a slight offset in depth. */
	GLfloat grey[4] = {0.5, 0.5, 0.5, 1};
	glUniform4fv(view15_uniform_color, 1, grey);

	glEnable(GL_DEPTH_TEST);

	if (view15_polygonoffset) {
		glPolygonOffset(1, 0);
		glEnable(GL_POLYGON_OFFSET_FILL);
	}

	glEnableVertexAttribArray(view15_attr_coord2d);
	glBindBuffer(GL_ARRAY_BUFFER, view15_vbo[0]);
	glVertexAttribPointer(view15_attr_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, view15_vbo[2]);
	glDrawElements(GL_TRIANGLES, 100 * 100 * 6, GL_UNSIGNED_SHORT, 0);

	glPolygonOffset(0, 0);
	glDisable(GL_POLYGON_OFFSET_FILL);

	/* Draw the grid, very bright */
	GLfloat bright[4] = {2, 2, 2, 1};
	glUniform4fv(view15_uniform_color, 1, bright);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, view15_vbo[1]);
	glDrawElements(GL_LINES, 100 * 101 * 4, GL_UNSIGNED_SHORT, 0);

	/* Stop using the vertex buffer object */
	glDisableVertexAttribArray(view15_attr_coord2d);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glutSwapBuffers();
    // TODO: something is wrong with view15
}

// TODO: add openal and load model
void view15_special(int key, int x, int y)
{
	switch (key) {
        case GLUT_KEY_F1:
            view15_interpolate = !view15_interpolate;
            // printf("Interpolation is now %s\n",
            //        view15_interpolate ? "on" : "off");
            break;
        case GLUT_KEY_F2:
            view15_clamp = !view15_clamp;
            // printf("Clamping is now %s\n", view15_clamp ? "on" : "off");
            break;
        case GLUT_KEY_F3:
            view15_rotate = !view15_rotate;
            // printf("Rotation is now %s\n", view15_rotate ? "on" : "off");
            break;
        case GLUT_KEY_F4:
            view15_polygonoffset = !view15_polygonoffset;
            // printf("Polygon offset is now %s\n",
            //        view15_polygonoffset ? "on" : "off");
            break;
        case GLUT_KEY_LEFT:
            view15_offset_x -= 0.03;
            break;
        case GLUT_KEY_RIGHT:
            view15_offset_x += 0.03;
            break;
        case GLUT_KEY_UP:
            view15_offset_y += 0.03;
            break;
        case GLUT_KEY_DOWN:
            view15_offset_y -= 0.03;
            break;
        case GLUT_KEY_PAGE_UP:
            view15_scale *= 1.5;
            break;
        case GLUT_KEY_PAGE_DOWN:
            view15_scale /= 1.5;
            break;
        case GLUT_KEY_HOME:
            view15_offset_x = 0.0;
            view15_offset_y = 0.0;
            view15_scale = 1.0;
            break;
	}

	glutPostRedisplay();
}

void view15_freeResources()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_POLYGON_OFFSET_FILL);

    glDeleteProgram(view15_program);
    glDeleteBuffers(3, view15_vbo);
    glDeleteTextures(1, &view15_texture_id);
}

void view15_entry(Window *window)
{
    window->program = view15_program;
    window->display = view15Display;
    window->entry = viewEntry;
    window->init = view15_initResources;
    window->free = view15_freeResources;
    window->special = view15_special;
}
