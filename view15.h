#ifndef VIEW15_H_
#define VIEW15_H_

#include <GL/glew.h>

extern GLuint view15_program;

int view15_initResources();
void view15_freeResources();
void view15Display();
void view15_special(int key, int x, int y);

#endif /* VIEW15_H_ */
