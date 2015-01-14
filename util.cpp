#include <iostream>
#include <fstream>
#include "util.h"

using namespace std;

const GLuint WINDOWN_BOARDER_GAP = 6;
const GLuint SUB_WINDOW_COL = 5, SUB_WINDOW_ROW = 5,
    SUB_WINDOW_WIDTH = 128, SUB_WINDOW_HEIGHT = 128;

const GLuint MAIN_WINDOW_WIDTH = SUB_WINDOW_WIDTH * SUB_WINDOW_COL +
    (SUB_WINDOW_COL + 1) * WINDOWN_BOARDER_GAP;
const GLuint MAIN_WINDOW_HEIGHT = SUB_WINDOW_HEIGHT * SUB_WINDOW_ROW +
    (SUB_WINDOW_ROW + 1) * WINDOWN_BOARDER_GAP;

// TODO: make as c++ class
// TODO: use boost

GLuint sphere(float radius, int slices, int stacks) {
    GLuint vbo;
    int n = 2 * (slices + 1) * stacks;
    int i = 0;

    // TODO: n dynamic right?
    glm::vec3 points[n];

    for (float theta = -M_PI / 2; theta < M_PI / 2 - 0.0001;
         theta += M_PI / stacks) {

        for (float phi = -M_PI; phi <= M_PI + 0.0001;
             phi += 2 * M_PI / slices) {
            points[i++] = glm::vec3(cos(theta) * sin(phi),
                                    -sin(theta),
                                    cos(theta) * cos(phi));

            points[i++] = glm::vec3(cos(theta + M_PI / stacks) * sin(phi),
                                    -sin(theta + M_PI / stacks),
                                    cos(theta + M_PI / stacks) * cos(phi));
        }
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof points, points, GL_STATIC_DRAW);

    return vbo;
}

glm::vec3 get_arcball_vector(int x, int y)
{
    glm::vec3 P = glm::vec3(1.0 * x / SUB_WINDOW_WIDTH * 2 - 1.0,
                            1.0 * y / SUB_WINDOW_HEIGHT * 2 - 1.0,
                            0);
    P.y = -P.y;

    float OP_squared = P.x * P.x + P.y * P.y;

    if (OP_squared <= 1 * 1) {
        P.z = sqrt(1 * 1 - OP_squared);
    } else {
        P = glm::normalize(P);  // nearest point
    }

    return P;
}

GLint get_uniform(GLuint program, const char *name) {
	GLint uniform = glGetUniformLocation(program, name);
	if(uniform == -1)
		fprintf(stderr, "Could not bind uniform %s\n", name);
	return uniform;
}

char *load_file(const char *path)
{
    char *content = NULL;

    ifstream  glslFile(path);
    if (glslFile.is_open()){
        glslFile.seekg (0, ios::end);
        streampos size = glslFile.tellg();
        // TODO: this is ugly
        streamoff extraSpace = 1;

        content = new char[size + extraSpace];
        glslFile.seekg(0, ios::beg);
        glslFile.read(content, size);
        content[size] = '\0';
        glslFile.close();
    } else {
        cerr << "can not load file: " << path << endl;
    }

    return content;

    // FILE *file_handler = fopen(path, "r");
    // if (file_handler == NULL){
    //     return NULL;
    // }

    // if (fseek(file_handler, 0, SEEK_END) == -1){
    //     return NULL;
    // }

    // long size = ftell(file_handler);
    // if (size == -1){
    //     return NULL;
    // }

    // if (fseek(file_handler, 0, SEEK_SET) == -1){
    //     return NULL;
    // }

    // char *content = (char *)malloc(size * sizeof(char) + 1);
    // if (content == NULL){
    //     return NULL;
    // }

    // fread(content, 1, (size_t)size, file_handler);

    // if (ferror(file_handler)){
    //     free(content);
    //     fclose(file_handler);
    //     return NULL;
    // }

    // fclose(file_handler);
    // content[size] = '\0';

    // return content;
}

void printLog(GLuint object)
{
    char *logBuffer = NULL;
    GLint logLength;
    if (glIsShader(object)){
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);
        logBuffer = (char *)malloc(sizeof(char) * logLength);
        glGetShaderInfoLog(object, logLength, NULL, logBuffer);
    } else if (glIsProgram(object)){
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &logLength);
        logBuffer = (char *)malloc(sizeof(char) * logLength);
        glGetProgramInfoLog(object, logLength, NULL, logBuffer);
    } else {
        return;
    }

    cerr << "pringLog : " << logBuffer << endl;
    free(logBuffer);
}

GLuint create_shader(const char *path, GLenum type)
{
    GLuint shader = glCreateShader(type);
    char *shader_src = load_file(path);
    if (shader_src == NULL){
        cerr << "load file " << path << "failed" << endl;
        return 0;
    }

    char *sources[2] = {
        (char *)"#version 130\n",
        shader_src
    };

    glShaderSource(shader, 2, (const char **)&sources, NULL);
    free(shader_src);
    glCompileShader(shader);

    GLint status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status == 0){
        cerr << "compile shader failed: " << path << endl;
        printLog(shader);
        return 0;
    }

    return shader;
}

GLint get_attrib(GLuint program, const char *attr_name)
{
    GLint attr = glGetAttribLocation(program, attr_name);
    if (attr == -1){
        cerr << "glGetAttribLocation attr_name "
             << attr_name << " failed!" << endl;
        return -1;
    }

    return attr;
}

GLuint create_program(const char *vertex_path, const char *fragment_path)
{
    GLint status = GL_FALSE;
    GLuint program = glCreateProgram();
    GLuint shader = 0;
    if (vertex_path != NULL){
        // create vertex shader
        if ((shader = create_shader(vertex_path, GL_VERTEX_SHADER)) == -1){
            cerr << "can not create vertex shader" << endl;
            return -1;
        }
        glAttachShader(program, shader);
    } else {
        cerr << "vertex shader file is NULL" << endl;
    }

    if (fragment_path != NULL){
        // create fragment shader
        if ((shader = create_shader(fragment_path, GL_FRAGMENT_SHADER)) == -1){
            cerr << "can not create fragment shader" << endl;
            return -1;
        }
        glAttachShader(program, shader);
    } else {
        cerr << "fragment shader file is NULL" << endl;
    }

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == 0){
        cerr << "link shader failed" << endl;
        printLog(program);
        return -1;
    }

    return program;
}

void viewEntry(int state)
{
    if (state == GLUT_LEFT){
        glClearColor(1, 1, 1, 1);
    } else if (state == GLUT_ENTERED) {
        glClearColor(0.5, 0.5, 0.5, 1.0);
    }
}


void resetWindow(Window *window)
{
    window->window = -1;
    window->x = 0;
    window->y = 0;
    window->width = MAIN_WINDOW_WIDTH;
    window->height = MAIN_WINDOW_HEIGHT;
    window->display = NULL;
    window->entry = NULL;
    window->init = NULL;
    window->free = NULL;
    window->idle = NULL;
    window->reshape = NULL;
    window->special = NULL;
    window->specialUp = NULL;
    window->keyboard = NULL;
    window->motion = NULL;
    window->mouse = NULL;
    window->internalMouse = NULL;
    window->program = 0;
}

void mini_idle()
{
    glutPostRedisplay();

}

int mini_initWindow(int argc, char *argv[], Window *window)
{
    glutInit(&argc, argv);

    // GLUT_RGB is alias for GLUT_RGBA
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_ALPHA |
                        GLUT_DEPTH | GLUT_STENCIL);

    glutInitWindowSize(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
    window->window = glutCreateWindow("all in one");

    if (glewInit() != GLEW_OK){
        return 1;
    }

    // TODO: upper version, support opengl es
    if (!GLEW_VERSION_2_0) {
        cerr << "Error: your graphic card does not support OpenGL 2.0" << endl;
        return 1;
    }

    GLint max_units;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &max_units);
    if (max_units < 1) {
        cerr << "Your GPU does not have any vertex texture image units" << endl;
        return 1;
    }

    // TODO: check why it is not tranparent now
    glutSetWindowTitle("all in one");

    // TODO: seems not work
    glutSetIconTitle("icon");

    if ((*window->init)() != 0){
        cerr << "window->init failed" << endl;
        return -1;
    }

    glutDisplayFunc(window->display);

    if (window->keyboard != NULL){
        glutKeyboardFunc(window->keyboard);
    }

    if (window->reshape != NULL){
        glutReshapeFunc(window->reshape);
    }

    if (window->mouse != NULL){
        glutMouseFunc(window->mouse);
    }

    if (window->special != NULL){
        glutSpecialFunc(window->special);
    }

    if (window->specialUp != NULL){
        glutSpecialUpFunc(window->specialUp);
    }

    if (window->motion != NULL){
        glutMotionFunc(window->motion);
    }

    if (window->free != NULL){
        glutCloseFunc(window->free);
    }

    if (window->idle != NULL){
        glutIdleFunc(window->idle);
    } else {
        glutIdleFunc(mini_idle);
    }

    return 0;
}
