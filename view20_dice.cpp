#include "view20_dice.h"
#include "util.h"
#include <SOIL/SOIL.h>

GLuint view20_dice_program = 0;

GLint view20_attr_coord3d = -1, view20_attr_texcoord = -1,
    view20_uniform_texture = -1, view20_uniform_mvp = -1;

unsigned char *view20_map_texture = NULL;

GLuint view20_map_tbo_texture = 0, view20_map_vbo_texture = 0,
    view20_map_vbo = 0;

GLfloat view20_map_coor[24] =
{0, 0,
 -58, 0,
 -83, 0,
 -108, 0,
 -133, 0,
 -158, 0,
 -201, 0,
 -201, 26,
 -201, 52,
 0, 0,
 -124, 91,
 0, 0,
};

unsigned char *view20_dice_texture = NULL;

GLuint view20_dice_tbo_texture = 0, view20_dice_vbo_texture = 0,
    view20_dice_vbo = 0, view20_dice_ibo = 0;

bool dice_roll = true;
// NOTE: picture size 490x490,
// rectangle: 56, 26
// boarder width: 16
// with between small circle 5, circle with = 20
// 1th:
// 2th: (-83+5+20=-58, 0)
// 3th: (-108+5+20=-83, 0)
// 4th: (-133+5+20=-108, 0)
// 5th: (-158+5+20=-133, 0)
// 6th: (-201+28+5+10=-158, 0)
// 7th: 490/2 - 16 - 56/2 = 201, (-201, 0)
// 8th: (-201, 26)
// 9th: (-201, 52)
// 10th: ()
// 11th: (-201+56/2+26/2=-160, 52+52/2+26/2=91)
// 12th: (-160+26=-124, 91)
// 13th: ()
//
// TODO: Play video and record motion in opengl
// TODO: motion control is not good as that japanese man
int view20_initResources()
{
    view20_dice_program = create_program("glsl/map.20.v.glsl",
                                           "glsl/map.20.f.glsl");

    view20_attr_coord3d = get_attrib(view20_dice_program, "coord3d");
    view20_attr_texcoord = get_attrib(view20_dice_program, "texcoord");

    const char *uniform_name = "texture";
    view20_uniform_texture = get_uniform(view20_dice_program,
                                         uniform_name);
    if (view20_uniform_texture == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
        return 1;
    }

    uniform_name = "mvp";
    view20_uniform_mvp = get_uniform(view20_dice_program, uniform_name);
    if (view20_uniform_mvp == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
        return 1;
    }

    int texture_width = 0, texture_height = 0;

    // NOTE: handle the map
    // TODO: use this SOIL_load_OGL_texture
    // TODO: check the different between
    // AnAeroplaneChessboard.png and AnAeroplaneChessboard.bak.png
    view20_map_texture = SOIL_load_image("data/AnAeroplaneChessboard.png",
                                         &texture_width,
                                         &texture_height,
                                         NULL, 0);

    // printf ("%d %d\n", texture_width, texture_height);
    glGenTextures(1, &view20_map_tbo_texture);
    glBindTexture(GL_TEXTURE_2D, view20_map_tbo_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 texture_width, texture_height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, view20_map_texture);

	glGenBuffers(1, &view20_map_vbo_texture);
	glBindBuffer(GL_ARRAY_BUFFER, view20_map_vbo_texture);
    GLfloat map_texture_coord[] = {
        0, 0,
        1, 0,
        1, 1,

        0, 0,
        1, 1,
        0, 1,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(map_texture_coord),
                 map_texture_coord, GL_STATIC_DRAW);


	glGenBuffers(1, &view20_map_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, view20_map_vbo);
    GLfloat map_world_coord[] = {
        -1, -1, 0,
        1, -1, 0,
        1, 1, 0,

        -1, -1, 0,
        1, 1, 0,
        -1, 1, 0,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(map_world_coord),
                 map_world_coord, GL_STATIC_DRAW);

    // NOTE: handle the dice ---------------------------
    view20_dice_texture = SOIL_load_image("data/dice.png",
                                          &texture_width,
                                          &texture_height,
                                          NULL, 0);

    glGenTextures(1, &view20_dice_tbo_texture);
    glBindTexture(GL_TEXTURE_2D, view20_dice_tbo_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 texture_width, texture_height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, view20_dice_texture);

    GLfloat dice_world_coord[] = {
        // front
        -1.0, -1.0,  1.0,
        1.0, -1.0,  1.0,
        1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        // top
        -1.0,  1.0,  1.0,
        1.0,  1.0,  1.0,
        1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0,
        // back
        1.0, -1.0, -1.0,
        -1.0, -1.0, -1.0,
        -1.0,  1.0, -1.0,
        1.0,  1.0, -1.0,
        // bottom
        -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0,
        1.0, -1.0,  1.0,
        -1.0, -1.0,  1.0,
        // left
        -1.0, -1.0, -1.0,
        -1.0, -1.0,  1.0,
        -1.0,  1.0,  1.0,
        -1.0,  1.0, -1.0,
        // right
        1.0, -1.0,  1.0,
        1.0, -1.0, -1.0,
        1.0,  1.0, -1.0,
        1.0,  1.0,  1.0,
    };
    glGenBuffers(1, &view20_dice_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, view20_dice_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dice_world_coord),
                 dice_world_coord, GL_STATIC_DRAW);

    GLfloat unit = 0.165;
    GLfloat dice_texture_coord[8*6] = {
        // front NO.1
        0.0, 0.0,
        unit, 0.0,
        unit, 1.0,
        0.0, 1.0,

        // top
        unit, 0.0,
        2*unit, 0.0,
        2*unit, 1.0,
        unit, 1.0,

        // back NO.6
        5*unit, 0.0,
        6*unit, 0.0,
        6*unit, 1.0,
        5*unit, 1.0,

        // bottom
        3*unit, 0.0,
        4*unit, 0.0,
        4*unit, 1.0,
        3*unit, 1.0,

        // left
        4*unit, 0.0,
        5*unit, 0.0,
        5*unit, 1.0,
        4*unit, 1.0,

        // right
        2*unit, 0.0,
        3*unit, 0.0,
        3*unit, 1.0,
        2*unit, 1.0,
    };

    // for(int i = 0; i < 6; ++i){
    //     dice_texture_coord[i*8]   = i*unit;     dice_texture_coord[i*8+1] = 0.0;
    //     dice_texture_coord[i*8+2] = (i+1)*unit; dice_texture_coord[i*8+3] = 0.0;
    //     dice_texture_coord[i*8+4] = (i+1)*unit; dice_texture_coord[i*8+5] = 1.0;
    //     dice_texture_coord[i*8+6] = i*unit;     dice_texture_coord[i*8+7] = 1.0;
    // }


    glGenBuffers(1, &view20_dice_vbo_texture);
    glBindBuffer(GL_ARRAY_BUFFER, view20_dice_vbo_texture);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dice_texture_coord),
                 dice_texture_coord, GL_STATIC_DRAW);

    GLushort dice_world_elements[] = {
        // front
        0,  1,  2,
        2,  3,  0,
        // top
        4,  5,  6,
        6,  7,  4,
        // back
        8,  9, 10,
        10, 11,  8,
        // bottom
        12, 13, 14,
        14, 15, 12,
        // left
        16, 17, 18,
        18, 19, 16,
        // right
        20, 21, 22,
        22, 23, 20,
    };
    glGenBuffers(1, &view20_dice_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, view20_dice_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(dice_world_elements),
                 dice_world_elements, GL_STATIC_DRAW);

    return 0;
}

void view20Mouse(int button, int state, int x, int y)
{
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        dice_roll = !dice_roll;
        return;
    }
}

void view20Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(view20_dice_program);

    glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                     glm::vec3(0.0, 0.0, -3.0));

    // Y: -2.0
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 0.0),
                                 glm::vec3(0.0, 0.0, -4.0),
                                 glm::vec3(0.0, 1.0, 0.0));

    glm::mat4 projection = glm::perspective(45.0f,
                                            1.0f * 128 / 128,
                                            0.1f, 10.0f);

    glm::mat4 mvp = projection * view * model;

    glUniformMatrix4fv(view20_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, view20_map_tbo_texture);
    glUniform1i(view20_uniform_texture, 0); // GL_TEXTURE0

    // texture coordinate
	glEnableVertexAttribArray(view20_attr_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, view20_map_vbo_texture);
	glVertexAttribPointer(view20_attr_texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // world coordinate
	glEnableVertexAttribArray(view20_attr_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, view20_map_vbo);
	glVertexAttribPointer(view20_attr_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // NOTE: handle the dice----------------
    // animate
    float angle = glutGet(GLUT_ELAPSED_TIME)/5.0 * glm::radians(15.0);
    glm::mat4 anim =
        glm::rotate(glm::mat4(1.0f), angle*3.0f, glm::vec3(1, 0, 0)) *
        glm::rotate(glm::mat4(1.0f), angle*3.0f, glm::vec3(0, 1, 0)) *
        glm::rotate(glm::mat4(1.0f), angle*3.0f, glm::vec3(0, 0, 1));

    if (!dice_roll){
        anim = glm::mat4(1.0f);
    }

    // TODO: this matrix should be calculated once
    model = glm::scale(glm::translate(glm::mat4(1.0f),
                                      // Y: -1.4
                                      glm::vec3(0.0, 0.0, -3.0)),
                       glm::vec3(0.2, 0.2, 0.2));
    mvp = projection * view * model * anim;
    glUniformMatrix4fv(view20_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, view20_dice_tbo_texture);
    glUniform1i(view20_uniform_texture, 1); // GL_TEXTURE0

    // texture coordinate
	glBindBuffer(GL_ARRAY_BUFFER, view20_dice_vbo_texture);
	glVertexAttribPointer(view20_attr_texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // world coordinate
	glBindBuffer(GL_ARRAY_BUFFER, view20_dice_vbo);
	glVertexAttribPointer(view20_attr_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, view20_dice_ibo);
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(view20_attr_texcoord);
	glDisableVertexAttribArray(view20_attr_coord3d);

    glutSwapBuffers();
}

void view20_dice_freeResources()
{
    glDeleteProgram(view20_dice_program);

    // TODO: active and deactive texture unit
    glDeleteTextures(1, &view20_map_tbo_texture);
    glDeleteBuffers(1, &view20_map_vbo_texture);
    glDeleteBuffers(1, &view20_map_vbo);

    glDeleteTextures(1, &view20_dice_tbo_texture);
    glDeleteBuffers(1, &view20_dice_vbo_texture);
    glDeleteBuffers(1, &view20_dice_vbo);
    glDeleteBuffers(1, &view20_dice_ibo);
}

void view20_entry(Window *window)
{
    window->program = view20_dice_program;
    window->display = view20Display;
    window->entry = viewEntry;
    window->internalMouse = view20Mouse;
    window->init = view20_initResources;
    window->free = view20_dice_freeResources;
    window->cull_face = true;
}
