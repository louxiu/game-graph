#ifndef VIEW16_TEAPOT_H_
#define VIEW16_TEAPOT_H_

#include <GL/glew.h>

extern GLuint view16_teapot_program;

int view16_initResources();
void view16_teapot_freeResources();
void view16Idle();
void view16Display();
void view16Reshape(int width, int height);

#endif /* VIEW16_TEAPOT_H_ */
