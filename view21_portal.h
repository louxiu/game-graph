#ifndef VIEW21_PORTAL_H_
#define VIEW21_PORTAL_H_

#include <GL/glew.h>

extern GLuint view21_portal_program;

int view21_portal_initResources();
void view21_portal_freeResources();
void view21_portal_Display();
void view21onReshape(int width, int height);
void view21onMouse(int button, int state, int x, int y);
void view21onMotion(int x, int y);

extern int view21_last_mx, view21_last_my, view21_cur_mx, view21_cur_my;
extern bool view21_arcball_on;

#endif /* VIEW21_PORTAL_H_ */
