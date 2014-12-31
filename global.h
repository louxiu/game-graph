#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <GL/glew.h>

extern const GLuint WINDOWN_BOARDER_GAP;

extern const GLuint SUB_WINDOW_COL, SUB_WINDOW_ROW,
    SUB_WINDOW_WIDTH, SUB_WINDOW_HEIGHT;

extern const GLuint MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT;

extern GLfloat fps;

struct demo {
    const char* texture_filename;
    const char* vshader_filename;
    const char* fshader_filename;
};

struct point {
	GLfloat x;
	GLfloat y;
};
#endif /* GLOBAL_H_ */
