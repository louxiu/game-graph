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

// http://www.reddit.com/r/gamedev/comments/232n4o/how_to_write_material_geometry_and_mesh_classes/
// https://github.com/libgdx/libgdx/tree/master/gdx/src/com/badlogic/gdx/graphics/g3d
class Mesh {
  private:
  public:
    GLuint vbo_vertices, vbo_colors, vbo_normals,
        ibo_elements, vbo_texture_vertices;

    vector<glm::vec4> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec4> colors;
    vector<GLushort> elements;
    vector<glm::vec4> texture_vertices;

    /* TODO: */
    /* vector<glm::vec4> tangents; */

    Mesh();
    virtual ~Mesh();

    void upload();
};

#endif /* MESH_H_ */
