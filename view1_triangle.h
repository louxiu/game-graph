#ifndef VIEW1_TRIANGLE_H_
#define VIEW1_TRIANGLE_H_

#include <GL/glew.h>

extern GLuint view1_triangle_program;

int view1_triangle_initResources();
void view1TriangleDisplay();
void view1_triangle_freeResources();

/* TODO: modify the Mesh.h to include it? */
#endif /* VIEW1_TRIANGLE_H_ */
