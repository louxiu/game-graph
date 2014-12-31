#ifndef VIEW19_H_
#define VIEW19_H_

#include <GL/glew.h>

extern GLuint view19_program;

int view19_initResources();
void view19_freeResources();

void view19Idle();
void view19Display();
void view19Reshape(int width, int height);
void view19Special(int key, int x, int y);
void view19Mouse(int button, int state, int x, int y);

#endif /* VIEW19_H_ */
