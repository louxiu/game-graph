#ifndef VIEW12_H_
#define VIEW12_H_

#include <GL/glew.h>

extern GLuint view12_program;

int view12_initResources();
void view12Display();
void view12_special(int key, int x, int y);
void view12_freeResources();

#endif /* VIEW12_H_ */
