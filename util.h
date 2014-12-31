#ifndef UTIL_H_
#define UTIL_H_

#include <GL/glew.h>
/* Using the GLUT library for the base windowing setup */
#include <GL/freeglut.h>
#include <math.h>

/* TODO: find these header usage */
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

GLuint sphere(float radius, int slices, int stacks);
glm::vec3 get_arcball_vector(int x, int y);
char *load_file(const char * path);
GLint get_uniform(GLuint program, const char *name);
void printLog(GLuint object);
GLuint create_shader(const char *path, GLenum type);
GLint get_attrib(GLuint program, const char *attr_name);
GLuint create_program(const char *vertex_path, const char *fragment_path);

#endif /* UTIL_H_ */
