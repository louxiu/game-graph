#ifndef VIEW17_CUBE_H_
#define VIEW17_CUBE_H_

#include <GL/glew.h>

extern GLuint view17_cube_program;

int view17_initResources();
void view17_freeResources();
void view17Display();
void view17Idle();
void view17Reshape(int width, int height);

#endif /* VIEW17_CUBE_H_ */
