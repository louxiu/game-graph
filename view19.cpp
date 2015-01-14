#include "view19.h"
#include "util.h"

GLuint view19_program = 0;

#include <glm/gtx/closest_point.hpp>
// TODO: why redefine problem
#undef glm_gtx_closest_point
#include <glm/gtx/closest_point.inl>
#include "data/view19_res_texture.c"

int view19_screen_width = 800, view19_screen_height = 600;

GLuint view19_vbo_cube_vertices, view19_vbo_cube_texcoords;
GLuint view19_ibo_cube_elements;
GLuint view19_texture_id;

GLint view19_attribute_coord3d, view19_attribute_texcoord;
GLint view19_uniform_mvp, view19_uniform_mytexture, view19_uniform_color;

#define NCUBES 10
glm::vec3 view19_positions[3 * NCUBES];
glm::vec3 view19_rotspeeds[3 * NCUBES];
bool view19_highlight[NCUBES];

float view19_angle = 0;
float view19_camera_angle = 0;
glm::vec3 view19_camera_position(0.0, 2.0, 4.0);

int view19_initResources()
{
    glClearColor(1.0, 1.0, 1.0, 0);

    // TODO: these function on, index return valid
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	static const GLfloat cube_vertices[] = {
		// front
		-1.0, -1.0, +1.0,
		+1.0, -1.0, +1.0,
		+1.0, +1.0, +1.0,
		-1.0, +1.0, +1.0,
		// top
		-1.0, +1.0, +1.0,
		+1.0, +1.0, +1.0,
		+1.0, +1.0, -1.0,
		-1.0, +1.0, -1.0,
		// back
		+1.0, -1.0, -1.0,
		-1.0, -1.0, -1.0,
		-1.0, +1.0, -1.0,
		+1.0, +1.0, -1.0,
		// bottom
		-1.0, -1.0, -1.0,
		+1.0, -1.0, -1.0,
		+1.0, -1.0, +1.0,
		-1.0, -1.0, +1.0,
		// left
		-1.0, -1.0, -1.0,
		-1.0, -1.0, +1.0,
		-1.0, +1.0, +1.0,
		-1.0, +1.0, -1.0,
		// right
		+1.0, -1.0, +1.0,
		+1.0, -1.0, -1.0,
		+1.0, +1.0, -1.0,
		+1.0, +1.0, +1.0,
	};

	glGenBuffers(1, &view19_vbo_cube_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, view19_vbo_cube_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof cube_vertices, cube_vertices,
                 GL_STATIC_DRAW);

	static const GLfloat cube_texcoords[2*4*6] = {
		0.0, 0.0,  1.0, 0.0,  1.0, 1.0,  0.0, 1.0,
		0.0, 0.0,  1.0, 0.0,  1.0, 1.0,  0.0, 1.0,
		0.0, 0.0,  1.0, 0.0,  1.0, 1.0,  0.0, 1.0,
		0.0, 0.0,  1.0, 0.0,  1.0, 1.0,  0.0, 1.0,
		0.0, 0.0,  1.0, 0.0,  1.0, 1.0,  0.0, 1.0,
		0.0, 0.0,  1.0, 0.0,  1.0, 1.0,  0.0, 1.0,
	};

	glGenBuffers(1, &view19_vbo_cube_texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, view19_vbo_cube_texcoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof cube_texcoords, cube_texcoords,
                 GL_STATIC_DRAW);

	static const GLushort cube_elements[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,
		// bottom
		12, 13, 14,
		14, 15, 12,
		// left
		16, 17, 18,
		18, 19, 16,
		// right
		20, 21, 22,
		22, 23, 20,
	};

	glGenBuffers(1, &view19_ibo_cube_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, view19_ibo_cube_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof cube_elements, cube_elements,
                 GL_STATIC_DRAW);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &view19_texture_id);
	glBindTexture(GL_TEXTURE_2D, view19_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, view19_res_texture.width,
                 view19_res_texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 view19_res_texture.pixel_data);

	view19_program = create_program("glsl/cube.19.v.glsl",
                                           "glsl/cube.19.f.glsl");
	if(view19_program == 0)
		return 1;

	view19_attribute_coord3d = get_attrib(view19_program, "coord3d");
	view19_attribute_texcoord = get_attrib(view19_program, "texcoord");
	view19_uniform_mvp = get_uniform(view19_program, "mvp");
	view19_uniform_mytexture = get_uniform(view19_program, "mytexture");
	view19_uniform_color = get_uniform(view19_program, "color");

	if(view19_attribute_coord3d == -1 || view19_attribute_texcoord == -1 ||
       view19_uniform_mvp == -1 || view19_uniform_mytexture == -1 ||
       view19_uniform_color == -1) {
		return 1;
    }

	srand48(time(NULL));

	for(int i = 0; i < NCUBES; i++) {
		view19_positions[i] = glm::vec3((drand48() - 0.5) * 2,
                                        (drand48() - 0.5) * 2,
                                        (drand48() - 0.5) * 2);
		view19_rotspeeds[i] = glm::vec3(drand48() * 5, drand48() * 5, drand48() * 5);
	}

	return 0;
}

void view19Idle()
{
    // base 15° per second
	view19_angle = glutGet(GLUT_ELAPSED_TIME) /
        1000.0 * glm::radians(15.0);

	glutPostRedisplay();
}

void view19Display()
{
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(view19_program);
	glUniform1i(view19_uniform_mytexture, /*GL_TEXTURE*/ 0);
	glEnableVertexAttribArray(view19_attribute_coord3d);
	// Describe our vertices array to OpenGL
	// (it can't guess its format automatically)
	glBindBuffer(GL_ARRAY_BUFFER, view19_vbo_cube_vertices);
	glVertexAttribPointer(view19_attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(view19_attribute_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, view19_vbo_cube_texcoords);
	glVertexAttribPointer(view19_attribute_texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, view19_ibo_cube_elements);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glm::mat4 view = glm::lookAt(view19_camera_position,
                                 glm::vec3(0.0, 0.0, 0.0),
                                 glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 projection = glm::perspective(45.0f,
                                            1.0f*view19_screen_width/\
                                            view19_screen_height, 0.1f, 10.0f);

	static const GLfloat color_normal[4] = {1, 1, 1, 1};
	static const GLfloat color_view19_highlight[4] = {2, 2, 2, 1};

	for(int i = 0; i < NCUBES; i++) {
		glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f),
                                                    view19_positions[i]),
                                     glm::vec3(0.2, 0.2, 0.2));

		glm::mat4 anim =
            // X axis
			glm::rotate(glm::mat4(1.0f),
                        view19_angle * view19_rotspeeds[i].x,
                        glm::vec3(1, 0, 0)) *
            // Y axis
			glm::rotate(glm::mat4(1.0f),
                        view19_angle * view19_rotspeeds[i].y,
                        glm::vec3(0, 1, 0)) *
            // Z axis
			glm::rotate(glm::mat4(1.0f),
                        view19_angle * view19_rotspeeds[i].z,
                        glm::vec3(0, 0, 1));

		glm::mat4 mvp = projection * view * model * anim;
		glUniformMatrix4fv(view19_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

		if(view19_highlight[i]) {
			glUniform4fv(view19_uniform_color, 1, color_view19_highlight);
        } else {
			glUniform4fv(view19_uniform_color, 1, color_normal);
        }

		glStencilFunc(GL_ALWAYS, i + 1, -1);

		/* Push each element in buffer_vertices to the vertex shader */
		glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	}

	glDisableVertexAttribArray(view19_attribute_coord3d);
	glDisableVertexAttribArray(view19_attribute_texcoord);
	glutSwapBuffers();
}

void view19Reshape(int width, int height)
{
	view19_screen_width = width;
	view19_screen_height = height;
	glViewport(0, 0, view19_screen_width, view19_screen_height);
}

void view19Special(int key, int x, int y)
{
	switch(key) {
		case GLUT_KEY_LEFT:
			view19_camera_angle -= glm::radians(5.0);
			break;
		case GLUT_KEY_RIGHT:
			view19_camera_angle += glm::radians(5.0);
			break;
	}

	view19_camera_position = glm::rotateY(glm::vec3(0.0, 2.0, 4.0),
                                          view19_camera_angle);
}

// TODO: add drag function to drag the object
void view19Mouse(int button, int state, int x, int y)
{
	if (!(state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON)) {
		return;
    }

	/* Read color, depth and stencil index from the framebuffer */
	GLbyte color[4];
	GLfloat depth;
	GLuint index;

	glReadPixels(x, view19_screen_height - y - 1, 1, 1,
                 GL_RGBA, GL_UNSIGNED_BYTE, color);
	glReadPixels(x, view19_screen_height - y - 1, 1, 1,
                 GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	glReadPixels(x, view19_screen_height - y - 1, 1, 1,
                 GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

	printf("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, "\
           "depth %f, stencil index %u\n",
           x, y, color[0], color[1], color[2], color[3], depth, index);

	/* Convert from window coordinates to object coordinates */
	glm::mat4 view = glm::lookAt(view19_camera_position, glm::vec3(0.0, 0.0, 0.0),
                                 glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 projection = glm::perspective(45.0f,
                                            1.0f*view19_screen_width\
                                            /view19_screen_height,
                                            0.1f, 10.0f);
	glm::vec4 viewport = glm::vec4(0, 0,
                                   view19_screen_width, view19_screen_height);

	glm::vec3 wincoord = glm::vec3(x, view19_screen_height - y - 1, depth);
	glm::vec3 objcoord = glm::unProject(wincoord, view, projection, viewport);

	/* Which box is nearest to those object coordinates? */
	int closest_i = 0;

	for(int i = 1; i < NCUBES; i++) {
		if(glm::distance(objcoord, view19_positions[i]) <
           glm::distance(objcoord, view19_positions[closest_i]))
			closest_i = i;
	}

    // TODO: there is a block can not be selected
	// printf("Coordinates in object space: %f, %f, "\
    //        "%f, closest to center of box %d\n",
    //        objcoord.x, objcoord.y, objcoord.z, closest_i + 1);

	/* Ray casting */
	closest_i = -1;
	float closest_distance = 1.0 / 0.0; // infinity

	wincoord.z = 0.99;
	glm::vec3 point1 = view19_camera_position;
	glm::vec3 point2 = glm::unProject(wincoord, view, projection, viewport);

	for(int i = 0; i < NCUBES; i++) {
		glm::vec3 cpol = glm::closestPointOnLine(view19_positions[i], point1, point2);
		float dtol = glm::distance(view19_positions[i], cpol);
		float distance = glm::distance(view19_positions[i], view19_camera_position);
		if(dtol < 0.2 * sqrtf(3.0) && distance < closest_distance) {
			closest_i = i;
			closest_distance = distance;
		}
	}

	// printf("Closest along camera ray: %d\n", closest_i + 1);

    // TODO: segmentfalut here
	/* Toggle view19_highlighting of the selected object */
	if(index != 0 && index < NCUBES) {
		view19_highlight[index - 1] = !view19_highlight[index - 1];
    }
}

void view19_freeResources()
{
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

	glDeleteProgram(view19_program);
	glDeleteBuffers(1, &view19_vbo_cube_vertices);
	glDeleteBuffers(1, &view19_vbo_cube_texcoords);
	glDeleteBuffers(1, &view19_ibo_cube_elements);
	glDeleteTextures(1, &view19_texture_id);
}

void view19_entry(Window *window)
{
    window->program = view19_program;
    window->display = view19Display;
    window->entry = viewEntry;
    window->init = view19_initResources;
    window->free = view19_freeResources;
    window->idle = view19Idle;
    window->reshape = view19Reshape;
    window->special = view19Special;
}
