#ifndef VIEW7_SUZANNE_H_
#define VIEW7_SUZANNE_H_

#include <GL/glew.h>

extern GLuint view7_suzanne_program;

void view7SuzanneLogic();
void view7SuzanneDisplay();
void view7SuzanneMotion(int x, int y);
void view7SuzanneMouse(int button, int state, int x, int y);
void view7SuzanneSpecialUp(int key, int x, int y);
void view7SuzanneKeyboard(unsigned char key, int x, int y);
void view7SuzanneSpecial(int key, int x, int y);
int view7_suzanne_initResources();
void view7_suzanne_freeResources();

#endif /* VIEW7_SUZANNE_H_ */
