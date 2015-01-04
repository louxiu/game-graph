#include "view7_suzanne.h"

#include "Mesh.h"
#include "util.h"
#include "global.h"

GLuint view7_suzanne_program = 0;
static int last_mx = 0, last_my = 0, cur_mx = 0, cur_my = 0;
static bool arcball_on = 0;

// TODO: draw lighting box
// TODO: add function of lighting mode
static unsigned int last_ticks = 0;
Mesh view7_suzanne_mesh, view7_ground_mesh, view7_light_bbox_mesh;

// TODO: something wrong with camera move
GLint view7_suzanne_attr_coord = -1, view7_suzanne_attr_normal = -1,
    view7_suzanne_uniform_m = -1, view7_suzanne_uniform_v = -1,
    view7_suzanne_uniform_p = -1, view7_suzanne_uniform_m_3x3_inv_transp = -1,
    view7_suzanne_uniform_v_inv = -1;

GLuint view7_postproc_program = 0, view7_fbo = 0, view7_texture_unit = 0,
    view7_rbo_depth = 0, view7_vbo_fbo_vertices = 0;
GLint view7_postproc_attr_coord = -1, view7_postproc_uniform_fbo_texture = -1,
    view7_postproc_uniform_offset = -1;

// TODO: suzanne disappear problem
glm::mat4 view7_suzanne_world2camera =
    glm::lookAt(
        glm::vec3(0.0,  0.0, 4.0),   // eye
        glm::vec3(0.0,  0.0, 0.0),   // direction
        glm::vec3(0.0,  1.0, 0.0));  // up

glm::mat4 view7_suzanne_camera2screen =
    glm::perspective(45.0f, 1.0f * 128 / 128, 0.1f, 100.0f);

GLint view7_suzanne_rotX_direction = 0, view7_suzanne_rotY_direction = 0,
    view7_suzanne_rotZ_direction = 0;

GLfloat view7_suzanne_strife = 0, view7_suzanne_speed_factor = 1;
enum MODES {MODE_OBJECT, MODE_CAMERA, MODE_LIGHT, MODE_LAST}
    view7_suzanne_view_mode = MODE_OBJECT;

// TODO: show obj name and shader name on screen
GLuint view7_objects_index = 3;
GLuint view7_objects_num = 4;
const char *view7_objects[] = {
    "data/cone.9.obj",
    "data/two-sided.7.obj",
    "data/suzanne.7.obj",
    "data/sphere.9.obj",
};

GLuint view7_shaders_index = 3;
GLuint view7_shaders_num = 4;
const char *view7_vertices_shaders[] = {
    "glsl/phong.7.shading.v.glsl",
    "glsl/phong.7.shading.v.glsl",
    "glsl/gouraud.7.shading.v.glsl",
    "glsl/gouraud.7.shading-two-sided.v.glsl",
};

const char *view7_fragment_shaders[] = {
    "glsl/phong.7.shading.f.glsl",
    "glsl/phong.7.shading-two-sided.f.glsl",
    "glsl/gouraud.7.shading.f.glsl",
    "glsl/gouraud.7.shading-two-sided.f.glsl",
};

void view7SuzanneLogic()
{
    int delta_t = glutGet(GLUT_ELAPSED_TIME) - last_ticks;
    last_ticks = glutGet(GLUT_ELAPSED_TIME);

    float delta_transX = 0, delta_transY = 0, delta_transZ = 0;
    float delta_rotX = 0, delta_rotY = 0, delta_rotZ = 0;

    // NOTE: strife: left right, tranY; page up, down, tranZ; up down, tranX
    // NOTE:         left right, rotY; page up, down, tranZ; up down, rotX

    // TODO: the unit problem
    if (view7_suzanne_strife == 1) {
        // 3 units per second ?
        delta_transX = view7_suzanne_rotX_direction * delta_t /
            1000.0 * 3 * view7_suzanne_speed_factor;

        delta_transY = view7_suzanne_rotY_direction * delta_t /
            1000.0 * 3 * view7_suzanne_speed_factor;

        // 5 units per second
        delta_transZ = view7_suzanne_rotZ_direction * delta_t /
            1000.0 * 3 * view7_suzanne_speed_factor;
    } else {
        // 120° per second
        delta_rotX = -view7_suzanne_rotX_direction * delta_t /
            1000.0 * 3600 * view7_suzanne_speed_factor;

        // 120° per second ?
        delta_rotY =  view7_suzanne_rotY_direction * delta_t /
            1000.0 * 3600 * view7_suzanne_speed_factor;

        delta_rotZ = view7_suzanne_rotZ_direction * delta_t /
            1000.0 * 3600 * view7_suzanne_speed_factor;
    }

    // NOTE: strife: left right, tranY; page up, down, tranZ; up down, tranX
    // NOTE:         left right, rotY; page up, down, tranZ; up down, rotX

    // TODO: direction negative and positive
    if (view7_suzanne_view_mode == MODE_OBJECT) {
        if (view7_suzanne_strife) {
            view7_suzanne_mesh.object2world =
                glm::translate(view7_suzanne_mesh.object2world,
                               glm::vec3(delta_transX, 0.0, 0.0));

            view7_suzanne_mesh.object2world =
                glm::translate(view7_suzanne_mesh.object2world,
                               glm::vec3(0.0, delta_transY, 0.0));

            view7_suzanne_mesh.object2world =
                glm::translate(view7_suzanne_mesh.object2world,
                               glm::vec3(0.0, 0.0, delta_transZ));
        } else {
            // when in object mode, the object is moved
            // TODO: rotate around the world's x, y, z
            view7_suzanne_mesh.object2world =
                glm::rotate(view7_suzanne_mesh.object2world,
                            glm::radians(delta_rotX),
                            glm::vec3(1.0, 0.0, 0.0));

            view7_suzanne_mesh.object2world =
                glm::rotate(view7_suzanne_mesh.object2world,
                            glm::radians(delta_rotY),
                            glm::vec3(0.0, 1.0, 0.0));

            view7_suzanne_mesh.object2world =
                glm::rotate(view7_suzanne_mesh.object2world,
                            glm::radians(delta_rotZ),
                            glm::vec3(0.0, 0.0, 1.0));
        }

    } else if (view7_suzanne_view_mode == MODE_CAMERA) {
        // TODO: add cordinate to screen
        // TODO: make camera a class
        // http://www.tomdalling.com/blog/modern-opengl/04-cameras-vectors-and-input/
        // TODO: add HOME handler

        // Camera is reverse-facing, so reverse Z translation and X rotation.
        // Plus, the View matrix is the inverse of the camera2world (it's
        // world->camera), so we'll reverse the transformations.
        // Alternatively, imagine that you transform the world,
        // instead of positioning the camera.

        // TODO: move along new x, y, z
        glm::vec3 x_axis_camera =
            // glm::mat3(view7_suzanne_world2camera) *
            glm::vec3(1.0, 0.0, 0.0);

        glm::vec3 y_axis_camera =
            // glm::mat3(view7_suzanne_world2camera) *
            glm::vec3(0.0, 1.0, 0.0);

        glm::vec3 z_axis_camera =
            // glm::mat3(view7_suzanne_world2camera) *
            glm::vec3(0.0, 0.0, 1.0);

        if (view7_suzanne_strife) {
            view7_suzanne_world2camera =
                glm::translate(glm::mat4(1.0),
                               glm::vec3(delta_transX, 0.0, 0.0)) *
                view7_suzanne_world2camera;

            view7_suzanne_world2camera =
                glm::translate(glm::mat4(1.0),
                               glm::vec3(0.0, delta_transY, 0.0)) *
                view7_suzanne_world2camera;

            view7_suzanne_world2camera =
                glm::translate(glm::mat4(1.0),
                               glm::vec3(0.0, 0.0, delta_transZ)) *
                view7_suzanne_world2camera;
        } else {
            view7_suzanne_world2camera =
                glm::rotate(glm::mat4(1.0),
                            glm::radians(delta_rotX),
                            x_axis_camera) *
                view7_suzanne_world2camera;

            view7_suzanne_world2camera =
                glm::rotate(glm::mat4(1.0),
                            glm::radians(delta_rotY),
                            y_axis_camera) *
                view7_suzanne_world2camera;

            view7_suzanne_world2camera =
                glm::rotate(glm::mat4(1.0),
                            glm::radians(delta_rotZ),
                            z_axis_camera) *
                view7_suzanne_world2camera;
        }
    }

    // TODO: read opengl wikibooks Tutorial_drafts
    // TODO: arcball does not work properly. angle is too small.
    // Handle arcball
    if (cur_mx != last_mx || cur_my != last_my) {
        glm::vec3 va = get_arcball_vector(last_mx, last_my);
        glm::vec3 vb = get_arcball_vector(cur_mx, cur_my);
        float angle = acos(min(1.0f, glm::dot(va, vb)));
        glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
        glm::mat3 camera2object =
            glm::inverse(glm::mat3(view7_suzanne_world2camera) *
                         glm::mat3(view7_suzanne_mesh.object2world));

        glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
        view7_suzanne_mesh.object2world =
            glm::rotate(view7_suzanne_mesh.object2world,
                        angle, axis_in_object_coord);

        last_mx = cur_mx;
        last_my = cur_my;

        // cout << "angle: " << angle
        //      << "coord: " << glm::to_string(axis_in_object_coord) << endl;
    }
}

void view7SuzanneDisplay()
{
    // TODO: this is the problem
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    view7SuzanneLogic();

    bool useFrameBuffer = false;
    if (useFrameBuffer){
        // NOTE: draw something on the frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, view7_fbo);
    }

    // glClearColor(0.45, 0.45, 0.45, 1.0);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(view7_suzanne_program);
    glUniformMatrix4fv(view7_suzanne_uniform_v, 1, GL_FALSE,
                       glm::value_ptr(view7_suzanne_world2camera));
    glUniformMatrix4fv(view7_suzanne_uniform_p, 1, GL_FALSE,
                       glm::value_ptr(view7_suzanne_camera2screen));

    glm::mat4 v_inv = glm::inverse(view7_suzanne_world2camera);
    glUniformMatrix4fv(view7_suzanne_uniform_v_inv, 1, GL_FALSE,
                       glm::value_ptr(v_inv));

    view7_suzanne_mesh.draw(view7_suzanne_mesh,
                            view7_suzanne_attr_coord,
                            -1,
                            view7_suzanne_attr_normal,
                            -1,
                            -1,
                            view7_suzanne_uniform_m,
                            view7_suzanne_uniform_m_3x3_inv_transp);

    view7_ground_mesh.draw(view7_ground_mesh,
                           view7_suzanne_attr_coord,
                           -1,
                           view7_suzanne_attr_normal,
                           -1,
                           -1,
                           view7_suzanne_uniform_m,
                           view7_suzanne_uniform_m_3x3_inv_transp);

    view7_light_bbox_mesh.draw_bbox(view7_light_bbox_mesh,
                                    view7_suzanne_attr_coord,
                                    view7_suzanne_attr_normal,
                                    view7_suzanne_uniform_m,
                                    view7_suzanne_uniform_m_3x3_inv_transp);

    // TODO: post process not work
    if (useFrameBuffer) {
        // NOTE: Post-processing the frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(view7_postproc_program);
        // 3/4 of a wave cycle per second
        GLfloat move = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 2 * 3.14159 * .75;
        glUniform1f(view7_postproc_uniform_offset, move);

        glBindTexture(GL_TEXTURE_2D, view7_texture_unit);
        glUniform1i(view7_postproc_uniform_fbo_texture, /*GL_TEXTURE*/ 0);

        glEnableVertexAttribArray(view7_postproc_attr_coord);
        glBindBuffer(GL_ARRAY_BUFFER, view7_vbo_fbo_vertices);
        glVertexAttribPointer(
            view7_postproc_attr_coord,  // attribute
            2,                  // number of elements per vertex, here (x,y)
            GL_FLOAT,           // the type of each element
            GL_FALSE,           // take our values as-is
            0,                  // no extra data between each position
            0                   // offset of first element
                              );

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(view7_postproc_attr_coord);
    }

    glutSwapBuffers();
    // TODO: handle the reshape
}

// TODO: background tranparent
void view7SuzanneMotion(int x, int y)
{
    // if left button is pressed
    if (arcball_on) {
        cur_mx = x;
        cur_my = y;
    }
}

void view7SuzanneMouse(int button, int state, int x, int y)
{
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        arcball_on = true;
        last_mx = cur_mx = x;
        last_my = cur_my = y;
    } else {
        arcball_on = false;
    }
}

void view7SuzanneSpecialUp(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            view7_suzanne_rotX_direction = 0;
            break;
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
            view7_suzanne_rotY_direction = 0;
            break;
        case GLUT_KEY_PAGE_UP:
        case GLUT_KEY_PAGE_DOWN:
            view7_suzanne_rotZ_direction = 0;
            break;
    }
}

void view7SuzanneKeyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case '1':
        {
            view7_objects_index = ++view7_objects_index % view7_objects_num;
            break;
        }
        case '2':
        {
            // view7_shaders_index = ++view7_shaders_index % view7_shaders_num;
            break;
        }
        default:
            return ;
    }

    view7_suzanne_mesh.clear();
    view7_ground_mesh.clear();
    view7_light_bbox_mesh.clear();

    glDeleteProgram(view7_suzanne_program);

    view7_suzanne_initResources();
}

void view7_init_view()
{
    view7_suzanne_mesh.object2world = glm::mat4(1);
    view7_suzanne_world2camera = glm::lookAt(
        glm::vec3(0.0,  0.0, 4.0),   // eye
        glm::vec3(0.0,  0.0, 0.0),   // direction
        glm::vec3(0.0,  1.0, 0.0));  // up
}

void view7SuzanneSpecial(int key, int x, int y)
{
    int modifiers = glutGetModifiers();
    if ((modifiers & GLUT_ACTIVE_ALT) == GLUT_ACTIVE_ALT) {
        view7_suzanne_strife = 1;
    } else {
        view7_suzanne_strife = 0;
    }

    if ((modifiers & GLUT_ACTIVE_SHIFT) == GLUT_ACTIVE_SHIFT) {
        view7_suzanne_speed_factor = 0.1;
    } else {
        view7_suzanne_speed_factor = 1;
    }

    switch (key) {
        case GLUT_KEY_F1:
            view7_suzanne_view_mode = MODE_OBJECT;
            break;
        case GLUT_KEY_F2:
            view7_suzanne_view_mode = MODE_CAMERA;
            break;
        case GLUT_KEY_F3:
            // TODO: lift the seal
            view7_suzanne_view_mode = MODE_LIGHT;
            break;
        case GLUT_KEY_LEFT:
            view7_suzanne_rotX_direction = 1;
            break;
        case GLUT_KEY_RIGHT:
            view7_suzanne_rotX_direction = -1;
            break;
        case GLUT_KEY_UP:
            view7_suzanne_rotY_direction = 1;
            break;
        case GLUT_KEY_DOWN:
            view7_suzanne_rotY_direction = -1;
            break;
        case GLUT_KEY_PAGE_UP:
            view7_suzanne_rotZ_direction = -1;
            break;
        case GLUT_KEY_PAGE_DOWN:
            view7_suzanne_rotZ_direction = 1;
            break;
        case GLUT_KEY_HOME:
            view7_init_view();
            break;
        default:
            break;
    }
}

int view7_suzanne_initResources()
{
    if (view7_objects_index == 4){
        // TODO: load the generated sphere
        int stacks = 30;
        int slices = 30;

        // TODO: 2 * (slices + 1) * stacks;
        glm::vec3 points[2 * (30 + 1) * 30];
        int i = 0;
        for (float theta = -M_PI / 2; theta < M_PI / 2 - 0.0001;
             theta += M_PI / stacks) {

            for (float phi = -M_PI; phi <= M_PI + 0.0001;
                 phi += 2 * M_PI / slices) {

                points[i++] = glm::vec3(cos(theta) * sin(phi),
                                        -sin(theta),
                                        cos(theta) * cos(phi));

                points[i++] = glm::vec3(cos(theta + M_PI / stacks) * sin(phi),
                                        -sin(theta + M_PI / stacks),
                                        cos(theta + M_PI / stacks) * cos(phi));
            }
        }

    } else {
        view7_suzanne_mesh.load_obj(view7_objects[view7_objects_index],
                                    view7_suzanne_mesh);
        view7_suzanne_mesh.upload();
    }

    int GROUND_SIZE = 20;
    for (int i = -GROUND_SIZE/2; i < GROUND_SIZE/2; i++) {
        for (int j = -GROUND_SIZE/2; j < GROUND_SIZE/2; j++) {
            view7_ground_mesh.vertices.push_back(glm::vec4(i,   0.0,  j+1, 1.0));
            view7_ground_mesh.vertices.push_back(glm::vec4(i+1, 0.0,  j+1, 1.0));
            view7_ground_mesh.vertices.push_back(glm::vec4(i,   0.0,  j,   1.0));
            view7_ground_mesh.vertices.push_back(glm::vec4(i,   0.0,  j,   1.0));
            view7_ground_mesh.vertices.push_back(glm::vec4(i+1, 0.0,  j+1, 1.0));
            view7_ground_mesh.vertices.push_back(glm::vec4(i+1, 0.0,  j,   1.0));
            for (unsigned int k = 0; k < 6; k++)
                view7_ground_mesh.normals.push_back(glm::vec3(0.0, 1.0, 0.0));
        }
    }
    view7_ground_mesh.upload();

    glm::vec3 light_position = glm::vec3(0.0,  1.0,  2.0);
    view7_light_bbox_mesh.vertices.push_back(glm::vec4(-0.1, -0.1, -0.1, 0.0));
    view7_light_bbox_mesh.vertices.push_back(glm::vec4( 0.1, -0.1, -0.1, 0.0));
    view7_light_bbox_mesh.vertices.push_back(glm::vec4( 0.1,  0.1, -0.1, 0.0));
    view7_light_bbox_mesh.vertices.push_back(glm::vec4(-0.1,  0.1, -0.1, 0.0));
    view7_light_bbox_mesh.vertices.push_back(glm::vec4(-0.1, -0.1,  0.1, 0.0));
    view7_light_bbox_mesh.vertices.push_back(glm::vec4( 0.1, -0.1,  0.1, 0.0));
    view7_light_bbox_mesh.vertices.push_back(glm::vec4( 0.1,  0.1,  0.1, 0.0));
    view7_light_bbox_mesh.vertices.push_back(glm::vec4(-0.1,  0.1,  0.1, 0.0));
    view7_light_bbox_mesh.object2world = glm::translate(glm::mat4(1),
                                                        light_position);
    view7_light_bbox_mesh.upload();

    // TODO: this?
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* create back-buffer, used for post-processing */
    /* texture */
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &view7_texture_unit);
    glBindTexture(GL_TEXTURE_2D, view7_texture_unit);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SUB_WINDOW_COL, SUB_WINDOW_ROW,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    /* depth buffer */
    glGenRenderbuffers(1, &view7_rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, view7_rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
                          SUB_WINDOW_COL, SUB_WINDOW_ROW);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    /* framebuffer to link everything together */
    glGenFramebuffers(1, &view7_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, view7_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           view7_texture_unit, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, view7_rbo_depth);
    GLenum status;
    if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER))
        != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "glCheckFramebufferStatus: error 0x%x\n", status);
        return 1;
    } else {
        // fprintf(stderr, "GL_FRAMEBUFFER success\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLfloat fbo_vertices[] = {
        -1, -1,
        1, -1,
        -1,  1,
        1,  1,
    };
    glGenBuffers(1, &view7_vbo_fbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, view7_vbo_fbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices,
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /* framebuffer to link everything together */

    view7_suzanne_program = create_program(
        view7_vertices_shaders[view7_shaders_index],
        view7_fragment_shaders[view7_shaders_index]);

    // printf ("%s %s\n",
    //         view7_vertices_shaders[view7_shaders_index],
    //         view7_fragment_shaders[view7_shaders_index]);

    view7_suzanne_attr_coord = get_attrib(view7_suzanne_program, "v_coord");
    view7_suzanne_attr_normal = get_attrib(view7_suzanne_program, "v_normal");

    const char* uniform_name;
    uniform_name = "m";
    view7_suzanne_uniform_m = get_uniform(view7_suzanne_program,
                                          uniform_name);
    uniform_name = "v";
    view7_suzanne_uniform_v = get_uniform(view7_suzanne_program,
                                          uniform_name);

    uniform_name = "p";
    view7_suzanne_uniform_p = get_uniform(view7_suzanne_program,
                                          uniform_name);
    uniform_name = "m_3x3_inv_transp";
    view7_suzanne_uniform_m_3x3_inv_transp = get_uniform(
        view7_suzanne_program, uniform_name);

    uniform_name = "v_inv";
    view7_suzanne_uniform_v_inv = get_uniform(view7_suzanne_program,
                                              uniform_name);

    /* Post-processing */
    view7_postproc_program =
        create_program("glsl/postproc.7.v.glsl", "glsl/postproc.7.f.glsl");

    view7_postproc_attr_coord = get_attrib(view7_suzanne_program, "v_coord");

    uniform_name = "fbo_texture";
    view7_postproc_uniform_fbo_texture =
        get_uniform(view7_postproc_program, uniform_name);

    uniform_name = "offset";
    view7_postproc_uniform_offset =
        get_uniform(view7_postproc_program, uniform_name);

    return 0;
}

// TODO: check memory leak
void view7_suzanne_freeResources()
{
    glDeleteProgram(view7_suzanne_program);
    glDeleteProgram(view7_postproc_program);

    glDeleteBuffers(1, &view7_vbo_fbo_vertices);
    glDeleteRenderbuffers(1, &view7_rbo_depth);
    glDeleteTextures(1, &view7_texture_unit);
    glDeleteFramebuffers(1, &view7_fbo);
}
