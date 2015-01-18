#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <GL/glew.h>
#include <map>
#include <string>
using namespace std;

const unsigned int MAX_NAME_LEN = 10;

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

  public:
    Program(const char *vs_path, const char *fs_path);
    virtual ~Program();

    void set_uniform();
    void set_attrib(const char *attr_name, GLuint value);
    void use();

};

#endif /* PROGRAM_H_ */
