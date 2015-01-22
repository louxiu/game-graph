#include "Program.h"
#include <stdlib.h>

#include <fstream>
#include <iostream>

char *Program::load_file(const char *path)
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

void Program::printLog(GLuint object)
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

    ShaderAttribUnif attrib;

    for (int index = 0; index < params; index++){
        char name[MAX_NAME_LEN] = {'\0'};
        glGetActiveAttrib(this->program, index, MAX_NAME_LEN,
                          &attrib.length, &attrib.size, &attrib.type,
                          name);
        attrib.name = name;
        attrib.location = get_attrib(name);
        attr_map[name] = attrib;
        // cerr << attrib.name << endl;
    }
}

void Program::unset_attrib(const char *attr_name)
{
    string name(attr_name);
    ShaderAttribUnif attrib = this->attr_map[name];
    glDisableVertexAttribArray(attrib.location);
}

void Program::set_attrib(const char *attr_name, GLuint value)
{
    string name(attr_name);
    ShaderAttribUnif attrib = this->attr_map[name];

    glEnableVertexAttribArray(attrib.location);

    glBindBuffer(GL_ARRAY_BUFFER, value);
    glVertexAttribPointer(
        attrib.location, // attribute
        4,                 // number of elements per vertex, here (x,y)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
    GLint params = -1;
    glGetProgramiv(this->program, GL_ACTIVE_UNIFORMS, &params);

    ShaderAttribUnif unif;

    for (int index = 0; index < params; index++){
        char name[MAX_NAME_LEN] = {'\0'};
        glGetActiveUniform(this->program, index, MAX_NAME_LEN,
                           &unif.length, &unif.size, &unif.type,
                           name);
        unif.name = name;
        unif.location = get_uniform(name);
        this->unif_map[name] = unif;
        // cout << this->attr_map[name] << endl;
    }
}

void Program::set_uniform1i(const char *unif_name, GLint value)
{
    string name(unif_name);
    ShaderAttribUnif unif = this->unif_map[name];

    // TODO: need to disactive at proper place
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, value);

    glUniform1i(unif.location, 0);
}

// https://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
void Program::set_uniform1f(const char *unif_name, float value)
{
    string name(unif_name);
    ShaderAttribUnif unif = this->unif_map[name];

    glUniform1f(unif.location, value);
}

void Program::set_uniformMatrix4fv(const char *unif_name, GLsizei count,
                                   GLboolean transpose, float *value)
{
    string name(unif_name);
    ShaderAttribUnif unif = this->unif_map[name];
    glUniformMatrix4fv(unif.location, count, transpose, value);
}

GLint Program::get_uniform(const char *name)
{
	GLint uniform = glGetUniformLocation(this->program, name);

    if(uniform == -1){
		cerr << "Could not get uniform " << name << endl;
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
    init_uniform_location();
}

// void Program::bind_mesh(Mesh *mesh)
// {
//     if (mesh->vbo_vertices != 0) {
//         this->set_attrib(mesh->attr_v_name, mesh->vbo_vertices);
//     }

//     if (mesh->vbo_colors != 0) {
//         this->set_attrib(mesh->attr_c_name, mesh->vbo_colors);
//     }

//     if (mesh->vbo_texture_vertices != 0) {
//         this->set_attrib(mesh->attr_tv_name, mesh->vbo_texture_vertices);
//     }
// }

// void Program::unbind_mesh(Mesh *mesh)
// {
//     if (mesh->vbo_vertices != 0) {
//         this->unset_attrib(mesh->attr_v_name);
//     }

//     if (mesh->vbo_colors != 0) {
//         this->unset_attrib(mesh->attr_c_name);
//     }

//     if (mesh->vbo_texture_vertices != 0) {
//         this->unset_attrib(mesh->attr_tv_name);
//     }
// }

void Program::begin()
{
    glUseProgram(this->program);
}

void Program::end()
{
    glUseProgram(0);
}

Program::~Program()
{
    glDeleteProgram(this->program);
}

ostream& operator<< (ostream &out, ShaderAttribUnif &attribUnif)
{
    // Since operator<< is a friend of the Point class, we can access
    // Point's members directly.
    out << "name: " << attribUnif.name << " ";
    out << "type: " << attribUnif.type << " ";
    out << "size: " << attribUnif.size << " ";
    out << "location: " << attribUnif.location;

    return out;

}
