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

Program *program = NULL;
Mesh *mesh = NULL;
GLint attribute_coord2d = -1;

int init_resources()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);

    program = new Program("glsl/triangle.1.v.glsl",
                          "glsl/triangle.1.f.glsl");

    mesh = new Mesh();

    glm::vec4 triangle_vertices[3] = {
        glm::vec4(0.0, 0.8, 0.0, 1.0),
        glm::vec4(-0.8, -0.8, 0.0, 1.0),
        glm::vec4(0.8, -0.8, 0.0, 1.0)
    };

    for(int i = 0; i < sizeof(triangle_vertices); ++i){
        mesh->vertices.push_back(triangle_vertices[i]);
    }

    mesh->upload();

    const char* attribute_name = "coord2d";
    attribute_coord2d = program->get_attrib(attribute_name);

    return 1;
}

void onDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);

    program->use();

    glEnableVertexAttribArray(attribute_coord2d);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_vertices);
    glVertexAttribPointer(
        attribute_coord2d, // attribute
        4,                 // number of elements per vertex, here (x,y)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0);

    /* Push each element in buffer_vertices to the vertex shader */
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(attribute_coord2d);

    glutSwapBuffers();

    glutPostRedisplay();
}

void free_resources()
{
    delete program;
    // delete mesh;
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
