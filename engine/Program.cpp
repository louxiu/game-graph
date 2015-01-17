#include "Program.h"
#include <stdlib.h>

#include <fstream>
#include <iostream>
using namespace std;

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

GLuint Program::create_shader(const char *path, GLenum type)
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

// TODO: new style?
// https://www.opengl.org/wiki/Program_Introspection
void Program::init_attr_location()
{
    GLint params = -1;
    glGetProgramiv(this->program, GL_ACTIVE_ATTRIBUTES, &params);

    GLsizei length = 0;
    GLint size = -1;
    GLenum type;
    char name[10];

    for (int index = 0; index < params; index++){
        glGetActiveAttrib(this->program, index, 9,
                          &length, &size, &type, name);
        cerr << name << endl;
    }

    // GLsizei length​ = 0;
    // GLint size​ = -1;
    // GLenum type​;
    // char name​[10];

    // for (int index = 0; index < params; index++){
    //     glGetActiveAttrib​(this->program​, index​, 9,
    //                       &length​, &size​, &type​, name​);
    // }

    // glGetObjectParameterivA (shaderID, GL_OBJECT_ACTIVE_ATTRIBUTES, &count);
    // glGetActiveAttrib (shaderID, i, bufSize, &length, &size, &type, name);
}

void Program::set_attrib()
{

}

GLint Program::get_attrib(const char *attr_name)
{
    GLint attr = glGetAttribLocation(this->program, attr_name);

    if (attr == -1){
        cerr << "glGetAttribLocation attr_name "
             << attr_name << " failed!" << endl;
        return -1;
    }

    return attr;
}

void Program::init_uniform_location()
{
    // glGetObjectParameteriv (shaderID, GL_OBJECT_ACTIVE_UNIFORMS, &count);
    // glGetActiveUniform (shaderID, i, bufSize, &length, &size, &type, name);
}

void Program::set_uniform()
{

}

GLint Program::get_uniform(GLuint program, const char *name)
{
	GLint uniform = glGetUniformLocation(program, name);

    if(uniform == -1){
		cerr << "Could not bind uniform " << name << endl;
    }
	return uniform;
}


void Program::use()
{
    glUseProgram(this->program);
}

Program::Program(const char *vs_path, const char *fs_path)
{
    GLint status = GL_FALSE;
    this->program = glCreateProgram();
    if (vs_path != NULL){
        // create vertex shader
        if ((this->vs_shader = create_shader(vs_path, GL_VERTEX_SHADER)) == -1){
            cerr << "can not create vertex shader" << endl;
            return;
        }
        glAttachShader(this->program, this->vs_shader);
    } else {
        cerr << "vertex shader file is NULL" << endl;
    }

    if (fs_path != NULL){
        // create fragment shader
        if ((this->fs_shader = create_shader(fs_path, GL_FRAGMENT_SHADER)) == -1){
            cerr << "can not create fragment shader" << endl;
            return;
        }
        glAttachShader(this->program, this->fs_shader);
    } else {
        cerr << "fragment shader file is NULL" << endl;
    }

    glLinkProgram(this->program);
    glGetProgramiv(this->program, GL_LINK_STATUS, &status);
    if (status == 0){
        cerr << "link shader failed" << endl;
        printLog(this->program);
        return;
    }

    init_attr_location();
}

Program::~Program()
{
    glDeleteProgram(this->program);
}
