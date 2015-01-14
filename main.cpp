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
// TODO: problem in fps and mini-portal problem in different pc

// calculate frames speed
GLfloat fps = 0.0;
static unsigned int fps_frames = 0, fps_start = 0;

GLuint active_window = 0;
GLuint main_window = 0;

struct Window win_array[100];

// TODO: create subsub window and category
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
        glutDestroyWindow(win_array[i].window);
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
int initWindow(int argc, char *argv[])
{
    glutInit(&argc, argv);
    // TODO: glutInitContextVersion(3, 0);

    // GLUT_RGB is alias for GLUT_RGBA
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_ALPHA |
                        GLUT_DEPTH | GLUT_STENCIL);

    glutInitWindowSize(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
    main_window = glutCreateWindow("all in one");

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
            win_array[winNum].window = -1;
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
            win_array[winNum].program = 0;
        }
    }

    // TODO: make game crazy bird to list
    // TODO: add menu tips like model load
    // TODO: select subwindow not work
    // TODO: view7 mouse float not work
    // TODO: destroy window

    view1_entry(&win_array[0]);
    view2_entry(&win_array[1]);
    view3_entry(&win_array[2]);
    view4_entry(&win_array[3]);
    view5_entry(&win_array[4]);
    view6_entry(&win_array[5]);
    view7_entry(&win_array[6]);
    view8_entry(&win_array[7]);
    view9_entry(&win_array[8]);
    view10_entry(&win_array[9]);
    view11_entry(&win_array[10]);
    view12_entry(&win_array[11]);
    view13_entry(&win_array[12]);
    view14_entry(&win_array[13]);
    view15_entry(&win_array[14]);
    view16_entry(&win_array[15]);
    view17_entry(&win_array[16]);
    view18_entry(&win_array[17]);
    view19_entry(&win_array[18]);
    view20_entry(&win_array[19]);
    view21_entry(&win_array[20]);

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
        if ((*win_array[i].init)() != 0){
            continue;
        }

        glutDisplayFunc(win_array[i].display);
        glutEntryFunc(win_array[i].entry);

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

        if (win_array[i].free != NULL){
            glutCloseFunc(win_array[i].free);
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


// TODO: 1. declare init hook in sub program, in main only call the init.
// TODO: 2. study stencil and depth buffer, study glEnable. fov
