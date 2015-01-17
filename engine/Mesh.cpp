#include "Mesh.h"
// TODO: add namespace?
#include <iostream>

Mesh::Mesh()
 :vbo_vertices(0), vbo_colors(0), vbo_normals(0),
  ibo_elements(0), vbo_texture_vertices(0)
{

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
