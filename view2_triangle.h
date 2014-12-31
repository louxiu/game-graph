#ifndef VIEW2_TRIANGLE_H_
#define VIEW2_TRIANGLE_H_

#include <GL/glew.h>

extern GLuint view2_triangle_program;

int view2_triangle_initResources();
void view2TriangleDisplay();
void view2_triangle_freeResources();

#endif /* VIEW2_TRIANGLE_H_ */
