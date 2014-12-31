#ifndef VIEW11_H_
#define VIEW11_H_

#include <GL/glew.h>

extern GLuint view11_program;
int view11_initResources();
void view11Display();
void view11_special(int key, int x, int y);
void view11_freeResources();

#endif /* VIEW11_H_ */
