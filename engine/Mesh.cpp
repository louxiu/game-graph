#include "Mesh.h"
// TODO: add namespace?
#include <iostream>
#include <sstream>
#include <fstream>

Mesh::Mesh()
 :vbo_vertices(0), vbo_colors(0), vbo_normals(0),
  ibo_elements(0), vbo_texture_vertices(0)
{
    memset(attr_v_name, 0, 20);
    memset(attr_n_name, 0, 20);
    memset(attr_c_name, 0, 20);
    memset(attr_tv_name, 0, 20);
}

Mesh::Mesh(const char* filename)
{
    ifstream in(filename, ios::in);
    if (!in) { cerr << "Cannot open " << filename << endl; return; }

    string line;
    while (getline(in, line)) {
        if (line.substr(0,2) == "v ") {
            istringstream s(line.substr(2));
            glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
            vertices.push_back(v);
        }  else if (line.substr(0,2) == "f ") {
            istringstream s(line.substr(2));
            GLushort a,b,c;
            s >> a; s >> b; s >> c;
            a--; b--; c--;

            elements.push_back(a);
            elements.push_back(b);
            elements.push_back(c);
        } else if (line[0] == '#') {
            /* ignoring this line */
        } else { /* ignoring this line */ }
    }

    normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
    for (int i = 0; i < elements.size(); i+=3) {
        GLushort ia = elements[i];
        GLushort ib = elements[i+1];
        GLushort ic = elements[i+2];
        glm::vec3 normal = glm::normalize(
            glm::cross(glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
                       glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
        normals[ia] = normals[ib] = normals[ic] = normal;
    }
}

Mesh::~Mesh()
{
    if (vbo_vertices != 0) {
        glDeleteBuffers(1, &vbo_vertices);
    }
    if (vbo_colors != 0) {
        glDeleteBuffers(1, &vbo_colors);
    }
    if (vbo_normals != 0) {
        glDeleteBuffers(1, &vbo_normals);
    }
    if (ibo_elements != 0){
        glDeleteBuffers(1, &ibo_elements);
    }
    if (vbo_texture_vertices != 0){
        glDeleteBuffers(1, &vbo_texture_vertices);
    }
}

void Mesh::set_attr_v_name(const char *attr_v_name)
{
    strncpy(this->attr_v_name, attr_v_name, sizeof(this->attr_v_name) - 1);
}

void Mesh::set_attr_n_name(const char *attr_n_name)
{
    strncpy(this->attr_n_name, attr_n_name, sizeof(this->attr_n_name) - 1);
}

void Mesh::set_attr_c_name(const char *attr_c_name)
{
    strncpy(this->attr_c_name, attr_c_name, sizeof(this->attr_c_name) - 1);
}

void Mesh::set_attr_tv_name(const char *attr_tv_name)
{
    strncpy(this->attr_tv_name, attr_tv_name, sizeof(this->attr_tv_name) - 1);
}

/**
 * Store object vertices, normals and/or elements in graphic card
 * buffers
 */
void Mesh::upload()
{
    if (this->vertices.size() > 0) {
        glGenBuffers(1, &this->vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER,
                     this->vertices.size() * sizeof(this->vertices[0]),
                     this->vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (this->colors.size() > 0) {
        glGenBuffers(1, &this->vbo_colors);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_colors);
        glBufferData(GL_ARRAY_BUFFER,
                     this->colors.size() * sizeof(this->colors[0]),
                     this->colors.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (this->normals.size() > 0) {
        glGenBuffers(1, &this->vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
        glBufferData(GL_ARRAY_BUFFER,
                     this->normals.size() * sizeof(this->normals[0]),
                     this->normals.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (this->elements.size() > 0) {
        glGenBuffers(1, &this->ibo_elements);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     this->elements.size() * sizeof(this->elements[0]),
                     this->elements.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    if (this->texture_vertices.size() > 0) {
        glGenBuffers(1, &this->vbo_texture_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_texture_vertices);
        glBufferData(GL_ARRAY_BUFFER,
                     this->texture_vertices.size() *
                     sizeof(this->texture_vertices[0]),
                     this->texture_vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Mesh::bind(Program &program)
{
    // TODO: move to vertex class
    if (this->vbo_vertices != 0) {
        program.set_attrib(this->attr_v_name, this->vbo_vertices);
    }

    if (this->vbo_colors != 0) {
        program.set_attrib(this->attr_c_name, this->vbo_colors);
    }

    if (this->vbo_texture_vertices != 0) {
        program.set_attrib(this->attr_tv_name, this->vbo_texture_vertices);
    }
}

void Mesh::unbind(Program &program)
{
    if (this->vbo_vertices != 0) {
        program.unset_attrib(this->attr_v_name);
    }

    if (this->vbo_colors != 0) {
        program.unset_attrib(this->attr_c_name);
    }

    if (this->vbo_texture_vertices != 0) {
        program.unset_attrib(this->attr_tv_name);
    }
}

void Mesh::render(Program &program)
{
    this->bind(program);

    /* Push each element in buffer_vertices to the vertex shader */
    if (this->ibo_elements != 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
        glDrawElements(GL_TRIANGLES, this->elements.size() / sizeof(GLushort),
                       GL_UNSIGNED_SHORT, 0);
        // cout << this->elements.size() / sizeof(GLushort) << endl;
    } else {
        glDrawArrays(GL_TRIANGLES, 0, this->vertices.size() / sizeof(glm::vec4));
        // cout << this->vertices.size() / sizeof(glm::vec4) << endl;
    }

    this->unbind(program);
}

void Mesh::render_bbox()
{
    // TODO:
}
