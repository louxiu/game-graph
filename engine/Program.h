#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <GL/glew.h>

class Program {
  private:
    GLuint program;
    GLuint vs_shader, fs_shader;

    /* GLint attr_v_coord, attr_v_colors, attr_v_normal, attr_t_coord; */
    GLint attr_array;
    GLint unif_array;

    GLint get_uniform(GLuint program, const char *name);
    GLuint create_shader(const char *path, GLenum type);

    void init_attr_location();
    void init_uniform_location();

  public:
    Program(const char *vs_path, const char *fs_path);
    virtual ~Program();

    GLint get_attrib(const char *attr_name);

    void set_uniform();
    void set_attrib();
    void use();

};
#endif /* PROGRAM_H_ */
