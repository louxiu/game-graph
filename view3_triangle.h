#ifndef VIEW3_TRIANGLE_H_
#define VIEW3_TRIANGLE_H_

#include <GL/glew.h>

extern GLuint view3_triangle_program;

void view3TriangleDisplay();
int view3_triangle_initResources();
void view3_triangle_freeResources();

#endif /* VIEW3_TRIANGLE_H_ */
