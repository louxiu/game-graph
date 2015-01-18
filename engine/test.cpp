/**
 * From the OpenGL Programming wikibook: http://en.wikibooks.org/wiki/OpenGL_Programming
 * This file is in the public domain.
 * Contributors: Sylvain Beucler
 */
#include <stdio.h>
#include <stdlib.h>
/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>
/* Using the GLUT library for the base windowing setup */
#include <GL/freeglut.h>
#include <iostream>
using namespace std;

#include "Mesh.h"
#include "Program.h"
#include "Render.h"

Program *program = NULL;
Mesh *mesh = NULL;
Render *render = NULL;

int init_resources()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);

    program = new Program("triangle.1.v.glsl",
                          "triangle.1.f.glsl");

    mesh = new Mesh();

    glm::vec4 triangle_vertices[3] = {
        glm::vec4(0.0, 0.8, 0.0, 1.0),
        glm::vec4(-0.8, -0.8, 0.0, 1.0),
        glm::vec4(0.8, -0.8, 0.0, 1.0)
    };

    for(int i = 0; i < sizeof(triangle_vertices); ++i){
        mesh->vertices.push_back(triangle_vertices[i]);
    }

    mesh->set_attr_v_name("coord2d");

    mesh->upload();

    render = new Render(mesh, program);

    return 1;
}

void onDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);

    render->draw();

    glutSwapBuffers();
    glutPostRedisplay();
}

void free_resources()
{
    delete program;
    delete mesh;
    delete render;
}

// g++ test.cpp Mesh.cpp Program.cpp -g -lGL -lglut -lGLEW -I../SOIL -I/usr/include/freetype2/ -L../SOIL -lm -lSOIL -lfreetype
int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    // glutInitContextVersion(2,0);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("My First Triangle");

    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK) {
        cerr << "Error: " << glewGetErrorString(glew_status) << endl;
        return 1;
    }

    if (init_resources()) {
        glutDisplayFunc(onDisplay);
        glutMainLoop();
    }

    free_resources();

    return 0;
}
