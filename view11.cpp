#include "view11.h"

#include "Mesh.h"
#include "util.h"
#include "global.h"

#include "data/view11_res_texture.c"

GLuint view11_program = 0;
GLint view11_attr_coord2d, view11_uniform_offset_x,
    view11_uniform_scale_x, view11_uniform_sprite, view11_uniform_mytexture;

GLuint view11_texture_id = 0;
float view11_offset_x = 0.0, view11_scale_x = 1.0;
int view11_mode = 0;

GLuint view11_vbo = 0;

int view11_initResources()
{
	view11_program = create_program("glsl/graph.11.v.glsl",
                                    "glsl/graph.11.f.glsl");

    view11_attr_coord2d = get_attrib(view11_program, "coord2d");
	view11_uniform_offset_x = get_uniform(view11_program, "offset_x");
	view11_uniform_scale_x = get_uniform(view11_program, "scale_x");
	view11_uniform_sprite = get_uniform(view11_program, "sprite");
	view11_uniform_mytexture = get_uniform(view11_program, "mytexture");

    // TODO: move other enable to init
	/* Enable blending */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Upload the texture for our point sprites */
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &view11_texture_id);
	glBindTexture(GL_TEXTURE_2D, view11_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, view11_res_texture.width,
                 view11_res_texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 view11_res_texture.pixel_data);

	// Create the vertex buffer object
	glGenBuffers(1, &view11_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, view11_vbo);

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

	return 0;
}

// TODO: how to check the memory use of GPU
void view11Display()
{
    // TODO: what's this?
    // uniform lowp float sprite;
	glUseProgram(view11_program);
	glUniform1i(view11_uniform_mytexture, 0);

	glUniform1f(view11_uniform_offset_x, view11_offset_x);
	glUniform1f(view11_uniform_scale_x, view11_scale_x);

	glClear(GL_COLOR_BUFFER_BIT);

	/* Draw using the vertices in our vertex buffer object */
	glBindBuffer(GL_ARRAY_BUFFER, view11_vbo);

	glEnableVertexAttribArray(view11_attr_coord2d);
	glVertexAttribPointer(view11_attr_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);

	/* Push each element in buffer_vertices to the vertex shader */
	switch (view11_mode) {
        case 0:
            glUniform1f(view11_uniform_sprite, 0);
            glDrawArrays(GL_LINE_STRIP, 0, 2000);
            break;
        case 1:
            glUniform1f(view11_uniform_sprite, 1);
            glDrawArrays(GL_POINTS, 0, 2000);
            break;
        case 2:
            glUniform1f(view11_uniform_sprite, view11_res_texture.width);
            glDrawArrays(GL_POINTS, 0, 2000);
            break;
	}

	glutSwapBuffers();
}

void view11_special(int key, int x, int y)
{
	switch (key) {
        case GLUT_KEY_F1:
            view11_mode = 0;
            // printf("Now drawing using lines.\n");
            break;
        case GLUT_KEY_F2:
            view11_mode = 1;
            // printf("Now drawing using points.\n");
            break;
        case GLUT_KEY_F3:
            view11_mode = 2;
            // printf("Now drawing using point sprites.\n");
            break;
        case GLUT_KEY_LEFT:
            view11_offset_x -= 0.1;
            break;
        case GLUT_KEY_RIGHT:
            view11_offset_x += 0.1;
            break;
        case GLUT_KEY_UP:
            view11_scale_x *= 1.5;
            break;
        case GLUT_KEY_DOWN:
            view11_scale_x /= 1.5;
            break;
        case GLUT_KEY_HOME:
            view11_offset_x = 0.0;
            view11_scale_x = 1.0;
            break;
	}

	glutPostRedisplay();
}

void view11_freeResources()
{
    glDeleteProgram(view11_program);
    glDeleteBuffers(1, &view11_vbo);
    glDeleteTextures(1, &view11_texture_id);
}

void view11_entry(Window *window)
{
    window->program = view11_program;
    window->display = view11Display;
    window->entry = viewEntry;
    window->init = view11_initResources;
    window->free = view11_freeResources;
    window->special = view11_special;
}
