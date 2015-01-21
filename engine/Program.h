#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <GL/glew.h>
#include <map>
#include <string>
#include "Mesh.h"

using namespace std;

const unsigned int MAX_NAME_LEN = 20;

class ShaderAttribUnif {
  public:
    string name;
    GLenum type;
    GLint size;
    GLsizei length;
    GLint location;

    friend ostream& operator<< (ostream &out, ShaderAttribUnif &attribUnif);
};

class Program {
  private:
    GLuint program;
    GLuint vs_shader, fs_shader;

    /* GLint attr_v_coord, attr_v_colors, attr_v_normal, attr_t_coord; */

    map<string, ShaderAttribUnif> attr_map;
    map<string, ShaderAttribUnif> unif_map;

    GLint get_uniform(const char *name);
    GLint get_attrib(const char *attr_name);

    GLuint create_shader(const char *path, GLenum type);

    void init_attr_location();
    void init_uniform_location();

    char *load_file(const char *path);
    void printLog(GLuint object);

  public:
    Program(const char *vs_path, const char *fs_path);
    virtual ~Program();
    void set_uniform1i(const char *unif_name, GLint value);
    void set_uniform1f(const char *unif_name, float value);
    void set_uniformMatrix4fv(const char *unif_name, GLsizei count,
                              GLboolean transpose, float *value);
    void set_attrib(const char *attr_name, GLuint value);
    void unset_attrib(const char *attr_name);
    void bind_mesh(Mesh *mesh);
    void unbind_mesh(Mesh *mesh);
    void use();
    void begin();
    void end();

};

#endif /* PROGRAM_H_ */
