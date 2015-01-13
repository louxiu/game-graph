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

typedef struct Window {
    GLint window;
    GLuint x, y, width, height;
    void (*display)();
    void (*mouse)(int button, int state, int x, int y);
    void (*internalMouse)(int button, int state, int x, int y);
    void (*entry)(int state);
    int  (*init)();
    void (*idle)();
    void (*reshape)(int width, int height);
    void (*special)(int key, int x, int y);
    void (*specialUp)(int key, int x, int y);
    void (*keyboard)(unsigned char key, int x, int y);
    void (*motion)(int x, int y);
    void (*free)();
    GLuint program;
} Window;

void viewEntry(int state);

#endif /* GLOBAL_H_ */
