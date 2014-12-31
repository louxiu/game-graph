#ifndef VIEW13_H_
#define VIEW13_H_

#include <GL/glew.h>

extern GLuint view13_program;

int view13_initResources();
void view13Display();
void view13_special(int key, int x, int y);
void view13_freeResources();

#endif /* VIEW13_H_ */
