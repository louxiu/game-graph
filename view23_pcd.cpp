#include "view23_pcd.h"
#include "util.h"
#include "SOIL/SOIL.h"
#include "data/view23_pcd_res_texture.c"

unsigned int screen_width = SUB_WINDOW_WIDTH, screen_height = SUB_WINDOW_HEIGHT;

GLuint view23_pcd_program = 0;
GLint view23_pcd_attr_coord2d, view23_pcd_attr_color,
    view23_pcd_uniform_offset_x, view23_pcd_uniform_offset_y,
     view23_pcd_uniform_sprite,
    view23_pcd_uniform_mytexture, view23_pcd_uniform_mvp;

GLuint view23_pcd_texture_id = 0;
float view23_pcd_offset_x = 0.0, view23_pcd_offset_y = 0.0,
    eye_pos = 40.0;

int point_number = 0;
glm::mat4 model = glm::mat4(1.0f);

// TODO: add this
glm::mat4 view23_pcd_world2camera =
    glm::lookAt(
        glm::vec3(0.0,  0.0, 4.0),   // eye
        glm::vec3(0.0,  0.0, 0.0),   // direction
        glm::vec3(0.0,  1.0, 0.0));  // up
glm::mat4 view23_pcd_camera2screen =
    glm::perspective(45.0f, 1.0f * 128 / 128, 0.1f, 100.0f);

GLuint view23_pcd_vbo = 0;
struct point3d {
	GLfloat x;
	GLfloat y;
	GLfloat z;

    GLubyte r;
	GLubyte g;
	GLubyte b;
};
static point3d graph[9000000];

int load_sin_data_example()
{
	for (int i = 0; i < 2000; i++) {
		float x = (i - 1000.0) / 100.0;
		graph[i].x = x;
		graph[i].y = sin(x * 10.0) / (1.0 + x * x);
		graph[i].z = 0;
        // printf ("%f %f %f\n", graph[i].x, graph[i].y, graph[i].z);
	}

    return 2000;
}

int load_pcd_data_example()
{
    FILE *file_handler = fopen("data/pcd_data_example", "r");
    // FILE *file_handler = fopen("data/pcd_data_text_real", "r");
	// Create our own temporary buffer
    float x = 0, y = 0, z = 0;

    int i = 0;
	// Fill it in just like an array
    while(fscanf(file_handler, "%f,%f,%f", &x, &y, &z) != EOF){
		graph[i].x = x;
        graph[i].z = y;
        graph[i].y = z;
        // printf ("%f %f %f\n", graph[i].x, graph[i].y, graph[i].z);
        i++;
	}

    fclose(file_handler);

    return i;
}

int load_pcd_color()
{
    int width = 2968, height = 3000;

    unsigned char * view23_pcd_color_array =
        SOIL_load_image("data/pcd_data_color.bmp",
                        &width, &height,
                        NULL, 0);

    if (view23_pcd_color_array != NULL){
        printf ("load color ok\n");

        int index = 0;
        unsigned char *array_index = view23_pcd_color_array;

        while (index < width * height) {
            graph[index].r = *(array_index++);
            graph[index].g = *(array_index++);
            graph[index].b = *(array_index++);

            index++;
        }

        printf ("load color index = %d, width * height = %d\n",
                index, width * height);

    }

    return width * height;
}

int load_pcd_data()
{
	FILE *fp  = fopen("data/pcd_data", "rb");
	if (fp == NULL)
	{
		printf("open file pcd_data failed\n");
	}

    fseek(fp, 0, SEEK_END);
	long nLen = ftell(fp);
	rewind(fp);

    float *p3Data = (float *)malloc(nLen);

    // TODO: no need nLen_1
    long nLen_1 = nLen / sizeof(float);
	nLen = fread(p3Data, sizeof(float), nLen_1, fp);

	float z;
	float *p3Data_1 = p3Data;
    int graph_index = 0;

    while (nLen > 0)
	{
		graph[graph_index].x = *p3Data_1++;
		graph[graph_index].y = *p3Data_1++;
		graph[graph_index].z = *p3Data_1++;

		nLen = nLen - 3;
        graph_index++;
	}

	fclose(fp);
	free(p3Data);

    return graph_index;
}

#include <iostream>
using namespace std;

int view23_pcd_initResources()
{
    glClearColor(0.0, 0.0, 0.0, 0);
	view23_pcd_program = create_program("glsl/pcd.23.v.glsl",
                                        "glsl/pcd.23.f.glsl");

    view23_pcd_attr_coord2d = get_attrib(view23_pcd_program, "coord2d");
    view23_pcd_attr_color = get_attrib(view23_pcd_program, "pic_color");

	view23_pcd_uniform_offset_x = get_uniform(view23_pcd_program, "offset_x");
	view23_pcd_uniform_offset_y = get_uniform(view23_pcd_program, "offset_y");
	view23_pcd_uniform_sprite = get_uniform(view23_pcd_program, "sprite");
	view23_pcd_uniform_mytexture = get_uniform(view23_pcd_program, "mytexture");
	view23_pcd_uniform_mvp = get_uniform(view23_pcd_program, "mvp");

    // TODO: move other enable to init
    glEnable(GL_CULL_FACE);
	/* Enable blending */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Upload the texture for our point sprites */
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &view23_pcd_texture_id);
	glBindTexture(GL_TEXTURE_2D, view23_pcd_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, view23_pcd_res_texture.width,
                 view23_pcd_res_texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 view23_pcd_res_texture.pixel_data);

	// Create the vertex buffer object
	glGenBuffers(1, &view23_pcd_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, view23_pcd_vbo);
    // int len = load_pcd_data();
    // int len = load_sin_data_example();
    point_number = load_pcd_data();
    // point_number = load_pcd_data_example();
    cout << point_number << endl;
    load_pcd_color();

    // load_pcd_data();
	// Tell OpenGL to copy our array to the buffer object
	glBufferData(GL_ARRAY_BUFFER,
                 point_number * (sizeof(GLfloat) * 3 + sizeof(GLubyte) * 3),
                 graph, GL_STATIC_DRAW);

	return 0;
}

// TODO: how to check the memory use of GPU
// TODO: use gpu
void view23_pcdDisplay()
{
    // TODO: what's this?
    // uniform lowp float sprite;
	glUseProgram(view23_pcd_program);
	glUniform1i(view23_pcd_uniform_mytexture, view23_pcd_texture_id);

    // TODO: show coordinate on the screen
    float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    // TODO: Swap the Y and Z
    // TODO: change position, direction, failed

    // https://www.opengl.org/discussion_boards/showthread.php/
    // 176535-Zooming-in-and-out-using-gluLookAt-gluPerspective

    // http://www.cs.kent.edu/~farrell/cg05/lectures/cg17.pdf
    glm::mat4 mvp = glm::mat4(1.0f);

    // model
    angle = 0;
    model =
        glm::rotate(glm::mat4(1.0f), angle,
                    glm::vec3(0.0, 1.0, 0.0)) *
        glm::translate(glm::mat4(1.0f), glm::vec3(-0.5, -0.5, -0.5));
    mvp = model * mvp;

    // view
    glm::mat4 view = glm::lookAt (
        glm::vec3(0.0,  0.0, eye_pos),   // eye
        glm::vec3(0.0,  0.0, -1.0),   // direction
        glm::vec3(0.0,  1.0, 0.0));  // up
    mvp = view * mvp;

    // TODO: default projection
    // projection
    float zNear = 0.01;
    float fovy = 60;
    glm::mat4 camera2screen =
        glm::perspective(fovy, 1.0f * screen_width / screen_height,
                         zNear, 100.0f);
    mvp = camera2screen * mvp;

    glUniformMatrix4fv(view23_pcd_uniform_mvp, 1, GL_FALSE,
                       glm::value_ptr(mvp));

	glUniform1f(view23_pcd_uniform_offset_x, view23_pcd_offset_x);
	glUniform1f(view23_pcd_uniform_offset_y, view23_pcd_offset_y);

	glClear(GL_COLOR_BUFFER_BIT);

	/* Draw using the vertices in our vertex buffer object */
	glBindBuffer(GL_ARRAY_BUFFER, view23_pcd_vbo);
	glEnableVertexAttribArray(view23_pcd_attr_coord2d);
	glVertexAttribPointer(view23_pcd_attr_coord2d, 3, GL_FLOAT,
                          GL_FALSE, sizeof(struct point3d), 0);

	glEnableVertexAttribArray(view23_pcd_attr_color);
	glVertexAttribPointer(view23_pcd_attr_color, 3, GL_UNSIGNED_BYTE,
                          GL_TRUE, sizeof(struct point3d),
                          (void*) offsetof(struct point3d, r));

	/* Push each element in buffer_vertices to the vertex shader */
    glUniform1f(view23_pcd_uniform_sprite, 1);
    glDrawArrays(GL_POINTS, 0, point_number);

	glutSwapBuffers();
}

void view23_pcd_special(int key, int x, int y)
{
	switch (key) {
        case GLUT_KEY_LEFT:
            view23_pcd_offset_x -= 0.1;
            break;

        case GLUT_KEY_RIGHT:
            view23_pcd_offset_x += 0.1;
            break;

        case GLUT_KEY_UP:
            view23_pcd_offset_y -= 1.0;
            break;

        case GLUT_KEY_DOWN:
            view23_pcd_offset_y += 1.0;
            break;

        case GLUT_KEY_HOME:
            view23_pcd_offset_x = 0.0;
            view23_pcd_offset_y = 0.0;
            eye_pos = 40.0;
            break;

        case GLUT_KEY_PAGE_UP:
            eye_pos -= 1.0;
            break;

        case GLUT_KEY_PAGE_DOWN:
            eye_pos += 1.0;
            break;
	}

	glutPostRedisplay();
}

void view23_pcd_freeResources()
{
	glDisable(GL_BLEND);

    glDeleteProgram(view23_pcd_program);
    glDeleteBuffers(1, &view23_pcd_vbo);
    glDeleteTextures(1, &view23_pcd_texture_id);
}

void view23_pcd_entry(Window *window)
{
    window->display = view23_pcdDisplay;
    window->entry = viewEntry;
    window->init = view23_pcd_initResources;
    window->free = view23_pcd_freeResources;
    window->special = view23_pcd_special;
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
