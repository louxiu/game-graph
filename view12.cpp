#include "view12.h"

#include "util.h"

GLuint view12_program = 0;
GLint view12_attr_coord1d = -1, view12_uniform_offset_x = -1;
GLint view12_uniform_scale_x =- 1, view12_uniform_mytexture = -1;
GLuint view12_texture_id = 0;

float view12_offset_x = 0.0, view12_scale_x = 1.0;

bool view12_interpolate = false, view12_clamp = false;
bool view12_showpoints = false;

GLuint view12_vbo = 0;

int view12_initResources()
{
    // TODO: texture lookups in the vertex shader
    // int vertex_texture_units;
    // glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &vertex_texture_units);

    // if(!vertex_texture_units) {
    //     fprintf(stderr, "Your graphics cards does not support texture lookups in the vertex shader!\n");
    //     // exit here or use another method to render the graph
    // }

	view12_program = create_program("glsl/graph.12.v.glsl",
                                           "glsl/graph.12.f.glsl");

	view12_attr_coord1d = get_attrib(view12_program, "coord1d");
	view12_uniform_offset_x = get_uniform(view12_program, "offset_x");
	view12_uniform_scale_x = get_uniform(view12_program, "scale_x");
	view12_uniform_mytexture = get_uniform(view12_program, "mytexture");

	if (view12_attr_coord1d == -1 || view12_uniform_offset_x == -1
        || view12_uniform_scale_x == -1 || view12_uniform_mytexture == -1) {
		return 1;
    }

	// Create our datapoints, store it as bytes
	GLbyte graph[2048];

	for (int i = 0; i < 2048; i++) {
		float x = (i - 1024.0) / 100.0;
		float y = sin(x * 10.0) / (1.0 + x * x);

		graph[i] = roundf(y * 128 + 128);
	}

	/* Upload the texture with our datapoints */
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &view12_texture_id);
	glBindTexture(GL_TEXTURE_2D, view12_texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 2048,
                 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, graph);


    // Create the vertex buffer object
	glGenBuffers(1, &view12_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, view12_vbo);

	// Create an array with only x values.
	GLfloat line[101];

	// Fill it in just like an array
	for (int i = 0; i < 101; i++) {
		line[i] = (i - 50) / 50.0;
	}

	// Tell OpenGL to copy our array to the buffer object
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

    // 	// Enable point size control in vertex shader
    // #ifndef GL_ES_VERSION_2_0
    // 	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    // #endif

	return 0;
}

// TODO: do the execise below the tutorial
void view12Display()
{
	glUseProgram(view12_program);
	glUniform1i(view12_uniform_mytexture, 0);

	glUniform1f(view12_uniform_offset_x, view12_offset_x);
	glUniform1f(view12_uniform_scale_x, view12_scale_x);

	glClear(GL_COLOR_BUFFER_BIT);

	/* Set texture wrapping mode */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    view12_clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);

	/* Set texture interpolation mode */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    view12_interpolate ? GL_LINEAR : GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    view12_interpolate ? GL_LINEAR : GL_NEAREST);

	/* Draw using the vertices in our vertex buffer object */
	glBindBuffer(GL_ARRAY_BUFFER, view12_vbo);

	glEnableVertexAttribArray(view12_attr_coord1d);
	glVertexAttribPointer(view12_attr_coord1d, 1, GL_FLOAT, GL_FALSE, 0, 0);

	/* Draw the line */
	glDrawArrays(GL_LINE_STRIP, 0, 101);

	/* Draw points as well, if requested */
	if (view12_showpoints) {
        // NOTE: zoom in, it is not very impressed
		glDrawArrays(GL_POINTS, 0, 101);
    }

	glutSwapBuffers();
}

void view12_special(int key, int x, int y)
{
	switch (key) {
        case GLUT_KEY_F1:
            view12_interpolate = !view12_interpolate;
            // printf("Interpolation is now %s\n",
            //        view12_interpolate ? "on" : "off");
            break;
        case GLUT_KEY_F2:
            view12_clamp = !view12_clamp;
            // printf("Clamping is now %s\n",
            //        view12_clamp ? "on" : "off");
            break;
        case GLUT_KEY_F3:
            view12_showpoints = !view12_showpoints;
            // printf("Showing points is now %s\n",
            //        view12_showpoints ? "on" : "off");
            break;
        case GLUT_KEY_LEFT:
            view12_offset_x -= 0.1;
            break;
        case GLUT_KEY_RIGHT:
            view12_offset_x += 0.1;
            break;
        case GLUT_KEY_UP:
            view12_scale_x *= 1.5;
            break;
        case GLUT_KEY_DOWN:
            view12_scale_x /= 1.5;
            break;
        case GLUT_KEY_HOME:
            view12_offset_x = 0.0;
            view12_scale_x = 1.0;
            break;
	}

	glutPostRedisplay();
}

void view12_freeResources()
{
    glDeleteProgram(view12_program);
    glDeleteBuffers(1, &view12_vbo);
    glDeleteTextures(1, &view12_texture_id);
}
