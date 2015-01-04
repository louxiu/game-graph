#ifndef VIEW20_DICE_H_
#define VIEW20_DICE_H_

#include <GL/glew.h>

extern GLuint view20_dice_program;

int view20_initResources();
void view20_dice_freeResources();
void view20Display();
void view20Mouse(int button, int state, int x, int y);

#endif /* VIEW20_DICE_H_ */
