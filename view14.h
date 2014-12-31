#ifndef VIEW14_H_
#define VIEW14_H_

#include <GL/glew.h>

extern GLuint view14_program;

int view14_initResources();
void view14_freeResources();
void view14Display();
void view14_special(int key, int x, int y);

#endif /* VIEW14_H_ */
