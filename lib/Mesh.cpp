#include "Mesh.h"
// TODO: add namespace?
#include <iostream>
#include <sstream>
#include <fstream>

Mesh::Mesh()
 :vbo_vertices(0), vbo_colors(0), vbo_normals(0), ibo_elements(0),
  vbo_texture(0), tbo_texture(0), object2world(glm::mat4(1)),
  texture(NULL), texture_width(0), texture_height(0)
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
    if (tbo_texture != 0){
        glDeleteTextures(1, &tbo_texture);
    }
    if (texture != NULL){
        free(texture);
    }
}

void Mesh::clear()
{
    /* this->~Mesh(); */
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
    if (tbo_texture != 0){
        glDeleteTextures(1, &tbo_texture);
    }
    if (texture != NULL){
        free(texture);
    }

    vbo_vertices = vbo_colors = vbo_normals = ibo_elements = 0;
    vbo_texture = tbo_texture = 0;
    object2world = glm::mat4(1);
    texture = NULL;
    texture_width = texture_height = 0;

    vertices.clear();
    colors.clear();
    normals.clear();
    elements.clear();
    texture_vertices.clear();

    /* printf ("%d %d %d %d %d \n", */
    /*         vertices.size(), */
    /*         colors.size(), */
    /*         normals.size(), */
    /*         elements.size(), */
    /*         texture_vertices.size()); */
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
    }

    if (this->colors.size() > 0) {
        glGenBuffers(1, &this->vbo_colors);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_colors);
        glBufferData(GL_ARRAY_BUFFER,
                     this->colors.size() * sizeof(this->colors[0]),
                     this->colors.data(), GL_STATIC_DRAW);
    }

    if (this->normals.size() > 0) {
        glGenBuffers(1, &this->vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
        glBufferData(GL_ARRAY_BUFFER,
                     this->normals.size() * sizeof(this->normals[0]),
                     this->normals.data(), GL_STATIC_DRAW);
    }

    if (this->elements.size() > 0) {
        glGenBuffers(1, &this->ibo_elements);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     this->elements.size() * sizeof(this->elements[0]),
                     this->elements.data(), GL_STATIC_DRAW);
    }

    if (this->texture_vertices.size() > 0) {
        glGenBuffers(1, &this->vbo_texture);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo_texture);
        glBufferData(GL_ARRAY_BUFFER,
                     this->texture_vertices.size() *
                     sizeof(this->texture_vertices[0]),
                     this->texture_vertices.data(), GL_STATIC_DRAW);
    }

    if (texture != NULL){
        glGenTextures(1, &this->tbo_texture);
        glBindTexture(GL_TEXTURE_2D, this->tbo_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     texture_width, texture_height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, texture);
    }
}

/* TODO: draw many object and optimize the performace */
/* TODO: move draw function to other object */
void Mesh::draw(Mesh &mesh,
                 GLint attr_v_coord,
                 GLint attr_v_colors,
                 GLint attr_v_normal,
                 GLint attr_t_coord,
                 GLint uniform_texture,
                 GLint uniform_m,
                 GLint uniform_m_3x3_inv_transp) {

    /* TODO: combine the vertices and normal buffer */
    if (mesh.vbo_vertices != 0 && attr_v_coord != -1) {
        glEnableVertexAttribArray(attr_v_coord);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_vertices);
        glVertexAttribPointer(
            attr_v_coord,  // attribute
            4,                  // number of elements per vertex, (x,y,z,w)
            GL_FLOAT,           // the type of each element
            GL_FALSE,           // take our values as-is
            0,                  // no extra data between each position
            0                   // offset of first element
                              );

        /* GLenum ret = glGetError(); */
        /* printf ("%d\n", ret); */
    }

    if (mesh.vbo_colors != 0 && attr_v_colors != -1) {
        glEnableVertexAttribArray(attr_v_colors);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_colors);
        glVertexAttribPointer(
            attr_v_colors,  // attribute
            4,                  // number of elements per vertex, (x,y,z,w)
            GL_FLOAT,           // the type of each element
            GL_FALSE,           // take our values as-is
            0,                  // no extra data between each position
            0                   // offset of first element
                              );

        /* GLenum ret = glGetError(); */
        /* printf ("%d\n", ret); */
    }

    if (mesh.vbo_normals != 0 && attr_v_normal != -1) {
        glEnableVertexAttribArray(attr_v_normal);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_normals);
        glVertexAttribPointer(
            attr_v_normal, // attribute
            3,                  // number of elements per vertex, (x,y,z)
            GL_FLOAT,           // the type of each element
            GL_FALSE,           // take our values as-is
            0,                  // no extra data between each position
            0                   // offset of first element
                              );
    }

    if (mesh.vbo_texture != 0 && attr_t_coord != -1) {
        glEnableVertexAttribArray(attr_t_coord);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_texture);
        glVertexAttribPointer(
            attr_t_coord,  // attribute
            4,                  // number of elements per vertex, (x,y,z,w)
            GL_FLOAT,           // the type of each element
            GL_FALSE,           // take our values as-is
            0,                  // no extra data between each position
            0                   // offset of first element
                              );

        /* GLenum ret = glGetError(); */
        /* printf ("%d\n", ret); */
    }

    /* TODO: segmentation fault */

    /* TODO: clean texture buffer */
    if (mesh.tbo_texture != 0 && uniform_texture != -1){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh.tbo_texture);
        // TODO: no need to glUniform1i every display?
        glUniform1i(uniform_texture, 0); // GL_TEXTURE0
    }

    if (uniform_m != -1){
        /* Apply object's transformation matrix */
        glUniformMatrix4fv(uniform_m, 1, GL_FALSE,
                           glm::value_ptr(mesh.object2world));
    }

    /* TODO: why tangent transformed ok? */
    if (uniform_m_3x3_inv_transp != -1){
        /* TODO: what's this? */
        /* Transform normal vectors with transpose of inverse of upper left
           3x3 model matrix (ex-gl_NormalMatrix): */
        glm::mat3 m_3x3_inv_transp =
            glm::transpose(glm::inverse(glm::mat3(mesh.object2world)));
        glUniformMatrix3fv(uniform_m_3x3_inv_transp, 1,
                           GL_FALSE, glm::value_ptr(m_3x3_inv_transp));
    }

    /* Push each element in buffer_vertices to the vertex shader */
    if (mesh.ibo_elements != 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo_elements);
        int size;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER,
                               GL_BUFFER_SIZE, &size);
        glDrawElements(GL_TRIANGLES, size / sizeof(GLushort),
                       GL_UNSIGNED_SHORT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
        /* GLenum ret = glGetError(); */
        /* printf ("%d\n", ret); */
    }

    if (mesh.vbo_vertices != 0) {
        glDisableVertexAttribArray(attr_v_coord);
    }

    if (mesh.vbo_colors != 0) {
        glEnableVertexAttribArray(attr_v_colors);
    }

    if (mesh.vbo_normals != 0) {
        glDisableVertexAttribArray(attr_v_normal);
    }

    if (mesh.vbo_texture != 0) {
        glDisableVertexAttribArray(attr_t_coord);
    }

    /* TODO: deactive tbo_texture*/
    /* TODO: vertex element clean */
}

/**
 * Draw object bounding box
 */
void Mesh::draw_bbox(Mesh &mesh,
                      GLint attr_v_coord,
                      GLint attr_v_normal,
                      GLint uniform_m,
                      GLint uniform_m_3x3_inv_transp) {

    /* TODO: check -1 */
    if (mesh.vertices.size() == 0)
        return;

    // Cube 1x1x1, centered on origin
    GLfloat vertices[] = {
        -0.5, -0.5, -0.5, 1.0,
        0.5, -0.5, -0.5, 1.0,
        0.5,  0.5, -0.5, 1.0,
        -0.5,  0.5, -0.5, 1.0,
        -0.5, -0.5,  0.5, 1.0,
        0.5, -0.5,  0.5, 1.0,
        0.5,  0.5,  0.5, 1.0,
        -0.5,  0.5,  0.5, 1.0,
    };
    GLuint vbo_vertices;
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                 vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLushort elements[] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };
    GLuint ibo_elements;
    glGenBuffers(1, &ibo_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements),
                 elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLfloat
        min_x, max_x,
        min_y, max_y,
        min_z, max_z;
    min_x = max_x = mesh.vertices[0].x;
    min_y = max_y = mesh.vertices[0].y;
    min_z = max_z = mesh.vertices[0].z;
    for (unsigned int i = 0; i < mesh.vertices.size(); i++) {
        if (mesh.vertices[i].x < min_x) min_x = mesh.vertices[i].x;
        if (mesh.vertices[i].x > max_x) max_x = mesh.vertices[i].x;
        if (mesh.vertices[i].y < min_y) min_y = mesh.vertices[i].y;
        if (mesh.vertices[i].y > max_y) max_y = mesh.vertices[i].y;
        if (mesh.vertices[i].z < min_z) min_z = mesh.vertices[i].z;
        if (mesh.vertices[i].z > max_z) max_z = mesh.vertices[i].z;
    }
    glm::vec3 size = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
    glm::vec3 center = glm::vec3((min_x+max_x)/2,
                                 (min_y+max_y)/2, (min_z+max_z)/2);
    glm::mat4 transform = glm::scale(glm::mat4(1), size) *
        glm::translate(glm::mat4(1), center);

    /* Apply object's transformation matrix */
    glm::mat4 m = mesh.object2world * transform;
    glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(m));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glEnableVertexAttribArray(attr_v_coord);
    glVertexAttribPointer(
        attr_v_coord,  // attribute
        4,                  // number of elements per vertex, here (x,y,z,w)
        GL_FLOAT,           // the type of each element
        GL_FALSE,           // take our values as-is
        0,                  // no extra data between each position
        0                   // offset of first element
                          );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT,
                   (GLvoid*)(4*sizeof(GLushort)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT,
                   (GLvoid*)(8*sizeof(GLushort)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(attr_v_coord);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &vbo_vertices);
    glDeleteBuffers(1, &ibo_elements);
}

void Mesh::load_obj(const char* filename,
                     vector<glm::vec4> &vertices,
                     vector<glm::vec3> &normals,
                     vector<GLushort> &elements)
{

    ifstream in(filename, ios::in);
    if (!in) { cerr << "Cannot open " << filename << endl; exit(1); }

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

void Mesh::load_obj(const string &filename, Mesh &mesh)
{
    load_obj(filename.c_str(), mesh.vertices,
             mesh.normals, mesh.elements);
}
