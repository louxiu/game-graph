#include <GL/glew.h>
// soemthing about window
#include <GL/freeglut.h>

#include <iostream>
#include <vector>
#include <string>

#include "Mesh.h"
#include "view1_triangle.h"
#include "view2_triangle.h"
#include "view3_triangle.h"
#include "view4_triangle.h"
#include "view5_cube.h"
#include "view6_cube.h"
#include "view7_suzanne.h"
#include "view8.h"
#include "view9_sphere.h"
#include "view10_cube.h"
#include "view11.h"
#include "view12.h"
#include "view13.h"
#include "view14.h"
#include "view15.h"
#include "view16_teapot.h"
#include "view17_cube.h"
#include "view18.h"
#include "view19.h"
#include "view20_dice.h"
#include "view21_portal.h"

#include "global.h"
#include "util.h"

using namespace std;
// TODO: different view from monkey

const GLuint WINDOWN_BOARDER_GAP = 6;
const GLuint SUB_WINDOW_COL = 5, SUB_WINDOW_ROW = 5,
    SUB_WINDOW_WIDTH = 128, SUB_WINDOW_HEIGHT = 128;

const GLuint MAIN_WINDOW_WIDTH = SUB_WINDOW_WIDTH * SUB_WINDOW_COL +
    (SUB_WINDOW_COL + 1) * WINDOWN_BOARDER_GAP;
const GLuint MAIN_WINDOW_HEIGHT = SUB_WINDOW_HEIGHT * SUB_WINDOW_ROW +
    (SUB_WINDOW_ROW + 1) * WINDOWN_BOARDER_GAP;

// calculate frames speed
GLfloat fps = 0.0;
static unsigned int fps_frames = 0, fps_start = 0;

GLuint active_window = 0;
GLuint main_window = 0;

struct Window {
    GLuint window;
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
    bool cull_face;
    GLuint program;
};

// SUB_WINDOW_COL * SUB_WINDOW_ROW
struct Window win_array[SUB_WINDOW_COL * SUB_WINDOW_ROW];

// TODO: add background, bbox, light
// TODO: add quad view
// TODO: reshape func
// TODO: design object class like object viewer
// TODO: use nice font to draw numbers
// TODO: load other obj than suzanne
// TODO: not use subwindow, make all in one window

int get_window_index(GLuint win)
{
    for(int i = 0; i < SUB_WINDOW_COL * SUB_WINDOW_ROW; ++i){
        if (win_array[i].window == win){
            return i;
        }
    }

    return 0;
}

// TODO: when click subwindow, main window will get event?
void mainDisplay()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glutSwapBuffers();
}

// TODO: add effect of slow max or min sub window
// TODO: add sub screen in sub window

// TODO: check G3D Innovation Engine
// http://g3d.sourceforge.net/

// TODO: add support to chinese utf8 font
// TODO: draw 3d mesh letter


// TODO: float over subwindow and shadow the boarder
// TODO: add drag window function
int initResources()
{
    return 0;
}

// TODO: check the test_res.c file
void windowIdle()
{
    /* FPS count */
    fps_frames++;
    int delta_t = glutGet(GLUT_ELAPSED_TIME) - fps_start;
    if (delta_t > 1000) {
        // cout << "fps: "<< 1000.0 * fps_frames / delta_t << endl;
        fps = 1000.0 * fps_frames / delta_t;
        fps_frames = 0;
        fps_start = glutGet(GLUT_ELAPSED_TIME);
    }

    // TODO: this is not good.
    for(int i = 0; i < SUB_WINDOW_COL * SUB_WINDOW_ROW; ++i){
        if (win_array[i].init == NULL){
            break;
        }

        // TODO: all window share the same idle
        // TODO: how to use idle
        if (active_window == 0 || active_window == win_array[i].window){
            glutSetWindow(win_array[i].window);
            glutShowWindow();
            glutPostRedisplay();

            if (win_array[i].idle != NULL){
                win_array[i].idle();
            }

        } else {
            glutSetWindow(win_array[i].window);
            glutHideWindow();
        }
    }
}

void freeResources()
{
    for(int i = 0; i < SUB_WINDOW_COL * SUB_WINDOW_ROW; ++i){
        if (win_array[i].free != NULL){
            win_array[i].free();
        }
    }
}

void viewMouse(int button, int state, int x, int y)
{
    int index = get_window_index(active_window);

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (active_window == 0){
            active_window = glutGetWindow();
            glutPositionWindow(0, 0);
            glutReshapeWindow(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
        } else {
            glutPositionWindow(win_array[index].x, win_array[index].y);
            glutReshapeWindow(SUB_WINDOW_WIDTH, SUB_WINDOW_HEIGHT);
            active_window = 0;
        }
    }

    if (win_array[index].internalMouse != NULL){
        win_array[index].internalMouse(button, state, x, y);
    }
}

// TODO: what's the function of glclear deapth and other
void viewEntry(int state)
{
    if (state == GLUT_LEFT){
        glClearColor(1, 1, 1, 1);
    } else if (state == GLUT_ENTERED) {
        glClearColor(0.5, 0.5, 0.5, 1.0);
    }
}

int initWindow(int argc, char *argv[])
{
    glutInit(&argc, argv);
    // TODO: glutInitContextVersion(3, 0);

    // GLUT_RGB is alias for GLUT_RGBA
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_ALPHA |
                        GLUT_DEPTH | GLUT_STENCIL);

    glutInitWindowSize(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
    main_window = glutCreateWindow("all in one");

    // TODO: check the use of these functions
    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_STENCIL_TEST);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (glewInit() != GLEW_OK){
        return 1;
    }

    // TODO: upper version, support opengl es
    if (!GLEW_VERSION_2_0) {
        cerr << "Error: your graphic card does not support OpenGL 2.0" << endl;
        return 1;
    }

	GLint max_units;
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &max_units);
	if (max_units < 1) {
		cerr << "Your GPU does not have any vertex texture image units" << endl;
		return 1;
	}

    // TODO: check why it is not tranparent now
    glutSetWindowTitle("all in one");
    // TODO: seems not work
    glutSetIconTitle("icon");

    // TODO: add clear color to window, different
    glutDisplayFunc(mainDisplay);
    glutIdleFunc(windowIdle);

    // TODO: make this as function hook, handle these by itself
    for(int i = 0; i < SUB_WINDOW_COL; ++i){
        for(int j = 0; j < SUB_WINDOW_ROW; ++j){
            int winNum = i * SUB_WINDOW_COL + j;
            win_array[winNum].window = 0;
            win_array[winNum].x = WINDOWN_BOARDER_GAP * (j + 1)
                + SUB_WINDOW_WIDTH * j;

            win_array[winNum].y = WINDOWN_BOARDER_GAP * (i + 1)
                + SUB_WINDOW_HEIGHT * i;

            win_array[winNum].width = SUB_WINDOW_WIDTH;
            win_array[winNum].height = SUB_WINDOW_HEIGHT;

            win_array[winNum].display = NULL;
            win_array[winNum].entry = NULL;
            win_array[winNum].init = NULL;
            win_array[winNum].free = NULL;
            win_array[winNum].idle = NULL;
            win_array[winNum].reshape = NULL;
            win_array[winNum].special = NULL;
            win_array[winNum].specialUp = NULL;
            win_array[winNum].keyboard = NULL;
            win_array[winNum].motion = NULL;
            win_array[winNum].mouse = viewMouse;
            win_array[winNum].internalMouse = NULL;
            win_array[winNum].cull_face = false;
            win_array[winNum].program = 0;
        }
    }

    // TODO: make game crazy bird to list
    // TODO: add menu tips like model load
    win_array[0].program = view1_triangle_program;
    win_array[0].display = view1TriangleDisplay;
    win_array[0].entry = viewEntry;
    win_array[0].init = view1_triangle_initResources;
    win_array[0].free = view1_triangle_freeResources;

    win_array[1].program = view2_triangle_program;
    win_array[1].display = view2TriangleDisplay;
    win_array[1].entry = viewEntry;
    win_array[1].init = view2_triangle_initResources;
    win_array[1].free = view2_triangle_freeResources;

    win_array[2].program = view3_triangle_program;
    win_array[2].display = view3TriangleDisplay;
    win_array[2].entry = viewEntry;
    win_array[2].init = view3_triangle_initResources;
    win_array[2].free = view3_triangle_freeResources;

    win_array[3].program = view4_triangle_program;
    win_array[3].display = view4TriangleDisplay;
    win_array[3].entry = viewEntry;
    win_array[3].init = view4_triangle_initResources;
    win_array[3].free = view4_triangle_freeResources;

    win_array[4].program = view5_cube_program;
    win_array[4].display = view5CubeDisplay;
    win_array[4].entry = viewEntry;
    win_array[4].init = view5_cube_initResources;
    win_array[4].free = view5_cube_freeResources;
    win_array[4].cull_face = true;

    win_array[5].program = view6_cube_program;
    win_array[5].display = view6CubeDisplay;
    win_array[5].entry = viewEntry;
    win_array[5].init = view6_cube_initResources;
    win_array[5].free = view6_cube_freeResources;
    win_array[5].cull_face = true;

    win_array[6].program = view7_suzanne_program;
    win_array[6].display = view7SuzanneDisplay;
    win_array[6].entry = viewEntry;
    win_array[6].init = view7_suzanne_initResources;
    win_array[6].free = view7_suzanne_freeResources;
    win_array[6].special = view7SuzanneSpecial;
    win_array[6].specialUp = view7SuzanneSpecialUp;
    win_array[6].keyboard = view7SuzanneKeyboard;
    win_array[6].motion = view7SuzanneMotion;
    win_array[6].internalMouse = view7SuzanneMouse;

    win_array[7].program = view8_program;
    win_array[7].display = view8Display;
    win_array[7].entry = viewEntry;
    win_array[7].init = view8_initResources;
    win_array[7].free = view8_freeResources;

    win_array[8].program = view9_sphere_program;
    win_array[8].display = view9SphereDisplay;
    win_array[8].entry = viewEntry;
    win_array[8].init = view9_Sphere_initResources;
    win_array[8].free = view9_Sphere_freeResources;
    win_array[8].keyboard = view9SphereKeyboard;
    win_array[8].cull_face = true;

    win_array[9].program = view10_cube_program;
    win_array[9].display = view10CubeDisplay;
    win_array[9].entry = viewEntry;
    win_array[9].init = view10_Cube_initResources;
    win_array[9].free = view10_Cube_freeResources;
    win_array[9].cull_face = true;

    win_array[10].program = view11_program;
    win_array[10].display = view11Display;
    win_array[10].entry = viewEntry;
    win_array[10].init = view11_initResources;
    win_array[10].free = view11_freeResources;
    win_array[10].special = view11_special;

    win_array[11].program = view12_program;
    win_array[11].display = view12Display;
    win_array[11].entry = viewEntry;
    win_array[11].init = view12_initResources;
    win_array[11].free = view12_freeResources;
    win_array[11].special = view12_special;

    win_array[12].program = view13_program;
    win_array[12].display = view13Display;
    win_array[12].entry = viewEntry;
    win_array[12].init = view13_initResources;
    win_array[12].free = view13_freeResources;
    win_array[12].special = view13_special;

    win_array[13].program = view14_program;
    win_array[13].display = view14Display;
    win_array[13].entry = viewEntry;
    win_array[13].init = view14_initResources;
    win_array[13].free = view14_freeResources;
    win_array[13].special = view14_special;

    win_array[14].program = view15_program;
    win_array[14].display = view15Display;
    win_array[14].entry = viewEntry;
    win_array[14].init = view15_initResources;
    win_array[14].free = view15_freeResources;
    win_array[14].special = view15_special;

    win_array[15].program = view16_teapot_program;
    win_array[15].display = view16Display;
    win_array[15].entry = viewEntry;
    win_array[15].init = view16_initResources;
    win_array[15].free = view16_teapot_freeResources;
    // win_array[15].special = view16_special;
    win_array[15].idle = view16Idle;
    win_array[15].reshape = view16Reshape;

    win_array[16].program = view17_cube_program;
    win_array[16].display = view17Display;
    win_array[16].entry = viewEntry;
    win_array[16].init = view17_initResources;
    win_array[16].free = view17_freeResources;
    win_array[16].idle = view17Idle;
    win_array[16].reshape = view17Reshape;

    win_array[17].program = view18_program;
    win_array[17].display = view18Display;
    win_array[17].entry = viewEntry;
    win_array[17].init = view18_initResources;
    win_array[17].free = view18_freeResources;

    win_array[18].program = view19_program;
    win_array[18].display = view19Display;
    win_array[18].entry = viewEntry;
    win_array[18].init = view19_initResources;
    win_array[18].free = view19_freeResources;
    win_array[18].idle = view19Idle;
    win_array[18].reshape = view19Reshape;
    win_array[18].special = view19Special;

    win_array[19].program = view20_dice_program;
    win_array[19].display = view20Display;
    win_array[19].entry = viewEntry;
    win_array[19].internalMouse = view20Mouse;
    win_array[19].init = view20_initResources;
    win_array[19].free = view20_dice_freeResources;
    win_array[19].cull_face = true;

    win_array[20].program = view21_portal_program;
    win_array[20].display = view21_portal_Display;
    win_array[20].entry = viewEntry;
    win_array[20].init = view21_portal_initResources;
    win_array[20].free = view21_portal_freeResources;
    win_array[20].reshape = view21onReshape;
    win_array[20].motion = view21onMotion;

    for(int i = 0; i < SUB_WINDOW_COL * SUB_WINDOW_ROW; ++i){
        if (win_array[i].init == NULL){
            break;
        }

        win_array[i].window = glutCreateSubWindow(main_window,
                                                  win_array[i].x,
                                                  win_array[i].y,
                                                  win_array[i].width,
                                                  win_array[i].height);
        // TODO: move to init
        glClearColor(1.0, 1.0, 1.0, 0);
        glutDisplayFunc(win_array[i].display);
        glutEntryFunc(win_array[i].entry);
        if ((*win_array[i].init)() != 0){
            continue;
        }

        // TODO: view8 disappear
        // TODO: glsl list all attr and uniform
        // http://stackoverflow.com/questions/440144/in-opengl-is-there-a-way-to-get-a-list-of-all-uniforms-attribs-used-by-a-shade
        // https://www.opengl.org/wiki/Program_Introspection

        if (win_array[i].keyboard != NULL){
            glutKeyboardFunc(win_array[i].keyboard);
        }

        if (win_array[i].reshape != NULL){
            glutReshapeFunc(win_array[i].reshape);
        }

        if (win_array[i].mouse != NULL){
            glutMouseFunc(win_array[i].mouse);
        }

        if (win_array[i].special != NULL){
            glutSpecialFunc(win_array[i].special);
        }

        if (win_array[i].specialUp != NULL){
            glutSpecialUpFunc(win_array[i].specialUp);
        }

        if (win_array[i].motion != NULL){
            glutMotionFunc(win_array[i].motion);
        }

        if (win_array[i].cull_face == true){
            glEnable(GL_CULL_FACE);
        }
    }

    // milliseconds
    fps_start = glutGet(GLUT_ELAPSED_TIME);

    return 0;
}

// TODO: how to control fps precisely
// TODO: add record vidoe file
// TODO: http://nehe.gamedev.net
// TODO: add a subwindow to play video
// http://nehe.gamedev.net/tutorial/playing_avi_files_in_opengl/23001/
int main(int argc, char *argv[])
{
    if (initWindow(argc, argv) != 0){
        return 0;
    }

    if (initResources() == 0){
        glutMainLoop();
    }

    freeResources();

    return 0;
}

// TODO: glutDestroyWindow

// TODO: check this
// http://feelmare.blogspot.jp/2012/01/sample-source-to-make-subwindow-in.html

// TODO: generate a sphere mannually
// http://www.andrewnoske.com/wiki/Generating_a_sphere_as_a_3D_mesh
// http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
// http://sol.gfxile.net/sphere/
// TODO: make view6 cube in blender and export as obj and add mtl
// TODO: make a more complete advanced obj loader
// TODO: obj loader https://github.com/syoyo/tinyobjloader
// TODO: nice site for mesh http://graphics.cs.williams.edu/data/meshes.xml
// TODO: uv, http://en.wikibooks.org/wiki/Blender_3D:_Noob_to_Pro/UV_Map_Basics
// TODO: read simple object viewer and glm object viewer souce code. wirte self
//       1. https://github.com/syoyo/tinyobjloader
//       2. https://github.com/tamato/simple-obj-loader/blob/master/objloader.cpp
//       3. http://people.cs.clemson.edu/~dhouse/courses/405/hw/hw6/hw6.html
//       4. https://github.com/devernay/glm
// TODO: continue the tutorial
