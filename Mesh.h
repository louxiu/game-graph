#ifndef MESH_H_
#define MESH_H_

#include <GL/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

/* TODO: make mesh more portable */
/* http://stackoverflow.com/questions/4873631/vertex-shader-attribute-mapping-in-glsl */
/* http://www.learnopengl.com/#!Model-Loading/Mesh */

/* TODO: split the implement and the declare */
/* TODO: add object left, right, top, front etc. move */
/* TODO: light object */
/* https://www.opengl.org/discussion_boards/showthread.php/142086-Object-Oriented-Programming-with-OpenGL */
/* TODO: add camera object */

/* TODO: optimize the performance */
/* https://www.opengl.org/discussion_boards/showthread.php/170055-glDrawElements-How-to-draw-multiple-objects */

#include <vector>
using namespace std;

class Mesh {
  private:
    GLuint vbo_vertices, vbo_colors, vbo_normals, ibo_elements;
    GLuint tbo_texture, vbo_texture;

  public:
    /* TODO: combine these attributes */
    vector<glm::vec4> vertices;
    vector<glm::vec4> colors;
    vector<glm::vec3> normals;
    vector<GLushort> elements;

    glm::mat4 object2world;

    vector<glm::vec4> texture_vertices;
    unsigned char *texture;
    int texture_width, texture_height;

    Mesh();
    virtual ~Mesh();

    void clear();
    void upload();
    static void draw(Mesh &mesh,
                     GLint attr_v_coord,
                     GLint attr_v_colors,
                     GLint attr_v_normal,
                     GLint attr_t_coord,
                     GLint uniform_texture,
                     GLint uniform_m,
                     GLint uniform_m_3x3_inv_transp);

    static void draw_bbox(Mesh &mesh,
                          GLint attr_v_coord,
                          GLint attr_v_normal,
                          GLint uniform_m,
                          GLint uniform_m_3x3_inv_transp);

    static void load_obj(const char* filename,
                         vector<glm::vec4> &vertices,
                         vector<glm::vec3> &normals,
                         vector<GLushort> &elements);

    static void load_obj(const string &filename, Mesh &mesh);
};

#endif /* MESH_H_ */
