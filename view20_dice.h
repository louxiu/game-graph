#ifndef VIEW20_DICE_H_
#define VIEW20_DICE_H_

#include <GL/glew.h>

extern GLuint view20_dice_program;
extern bool view20_dice_roll;

int view20_initResources();
void view20_dice_freeResources();
void view20Display();

#endif /* VIEW20_DICE_H_ */
