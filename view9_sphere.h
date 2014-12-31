#ifndef VIEW9_SPHERE_H_
#define VIEW9_SPHERE_H_

#include <GL/glew.h>

extern GLuint view9_sphere_program;
void view9SphereDisplay();
void view9SphereKeyboard(unsigned char key, int x, int y);
int view9_Sphere_initResources();
void view9_Sphere_freeResources();

#endif /* VIEW9_SPHERE_H_ */
