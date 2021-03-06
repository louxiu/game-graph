#ifndef MESH_H_
#define MESH_H_

#include <GL/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <vector>
using namespace std;
#include "Program.h"

class Mesh {
  private:

  public:
    GLuint vbo_vertices, vbo_colors, vbo_normals,
        ibo_elements, vbo_texture_vertices;

    char attr_v_name[20];
    char attr_n_name[20];
    char attr_c_name[20];
    char attr_tv_name[20];

    vector<glm::vec4> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec4> colors;
    vector<GLushort> elements;
    vector<glm::vec4> texture_vertices;

    /* TODO: */
    /* vector<glm::vec4> tangents; */

    Mesh();
    Mesh(const char* filename);
    virtual ~Mesh();

    void bind(Program &program);
    void unbind(Program &program);

    void set_attr_v_name(const char *attr_v_name);
    void set_attr_n_name(const char *attr_n_name);
    void set_attr_c_name(const char *attr_c_name);
    void set_attr_tv_name(const char *attr_tv_name);

    void upload();
    void render(Program &program);
    void render_bbox();
};

#endif /* MESH_H_ */
