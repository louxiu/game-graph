#include <iostream>
#include <fstream>
#include "global.h"
#include "util.h"

using namespace std;

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
    }

    if (fragment_path != NULL){
        // create fragment shader
        if ((shader = create_shader(fragment_path, GL_FRAGMENT_SHADER)) == -1){
            cerr << "can not create fragment shader" << endl;
            return -1;
        }
        glAttachShader(program, shader);
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
