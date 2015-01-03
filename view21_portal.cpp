#include "view21_portal.h"
#include "Mesh.h"
#include "global.h"
#include "util.h"

GLuint view21_portal_program = 0;
int view21_last_mx = 0, view21_last_my = 0,
    view21_cur_mx = 0, view21_cur_my = 0;
bool view21_arcball_on = 0;

Mesh main_mesh, ground_mesh, light_bbox_mesh;
Mesh portals[2];

static unsigned int last_ticks = 0;
// TODO: change with fullscreen, consider other view
unsigned int screen_width = SUB_WINDOW_WIDTH, screen_height = SUB_WINDOW_HEIGHT;

struct rect { int x,y,w,h; };
int rotY_direction = 0, rotX_direction = 0, transZ_direction = 0, strife = 0;
float speed_factor = 1;

#define GROUND_SIZE 20

static float zNear = 0.01;
static float fovy = 45;
GLint attr_v_coord;
GLint attr_v_normal;
GLint uniform_m, uniform_v, uniform_p;
GLint uniform_m_3x3_inv_transp, uniform_v_inv;

enum MODES { MODE_OBJECT, MODE_CAMERA, MODE_LIGHT, MODE_LAST }
    view_mode = MODE_CAMERA;
glm::mat4 transforms[MODE_LAST];

void init_view()
{
    main_mesh.object2world = glm::translate(glm::mat4(1), glm::vec3(-2, 1, 0))
        * glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0, 1, 0));
    transforms[MODE_CAMERA] = glm::lookAt(
        glm::vec3(0.0,  1.0, 6.0),   // eye
        glm::vec3(0.0,  1.0, 0.0),   // direction
        glm::vec3(0.0,  1.0, 0.0));  // up

    // transforms[MODE_CAMERA] = glm::lookAt(
    //   glm::vec3( 0.0,  1.0, -1.8),   // eye
    //   glm::vec3(-3.0,  1.0,  0.0),   // direction
    //   glm::vec3( 0.0,  1.0,  0.0));  // up
}

void create_portal(Mesh* portal, int screen_width, int screen_height,
                   float zNear, float fovy)
{
    portal->vertices.clear();
    portal->elements.clear();

    float aspect = 1.0 * screen_width / screen_height;
    float fovy_rad = fovy * M_PI / 180;
    float fovx_rad = fovy_rad / aspect;
    float dz = max(zNear/cos(fovx_rad), zNear/cos(fovy_rad));
    float dx = tan(fovx_rad) * dz;
    float dy = tan(fovy_rad) * dz;

    glm::vec4 portal_vertices[] = {
        glm::vec4(-1, -1, 0, 1),
        glm::vec4( 1, -1, 0, 1),
        glm::vec4(-1,  1, 0, 1),
        glm::vec4( 1,  1, 0, 1),

        glm::vec4(-(1+dx), -(1+dy), 0-dz, 1),
        glm::vec4( (1+dx), -(1+dy), 0-dz, 1),
        glm::vec4(-(1+dx),  (1+dy), 0-dz, 1),
        glm::vec4( (1+dx),  (1+dy), 0-dz, 1),
    };

    for (unsigned int i = 0; i < sizeof(portal_vertices)
             /sizeof(portal_vertices[0]); i++) {
        portal->vertices.push_back(portal_vertices[i]);
    }

    GLushort portal_elements[] = {
        0,1,2, 2,1,3,
        4,5,6, 6,5,7,
        0,4,2, 2,4,6,
        5,1,7, 7,1,3,
    };

    for (unsigned int i = 0; i < sizeof(portal_elements)
             /sizeof(portal_elements[0]); i++) {
        portal->elements.push_back(portal_elements[i]);
    }
}

/**
 * Checks whether the line defined by two points la and lb intersects
 * the portal.
 */
int portal_intersection(glm::vec4 la, glm::vec4 lb, Mesh* portal)
{
    if (la != lb) {  // camera moved
        // Check for intersection with each of the portal's 2 front triangles
        for (int i = 0; i < 2; i++) {
            // Portal coordinates in world view
            glm::vec4
                p0 = portal->object2world * portal->vertices[portal->elements[i*3+0]],
                p1 = portal->object2world * portal->vertices[portal->elements[i*3+1]],
                p2 = portal->object2world * portal->vertices[portal->elements[i*3+2]];

            // Solve line-plane intersection using parametric form
            glm::vec3 tuv =
                glm::inverse(
                    glm::mat3(glm::vec3(la.x - lb.x, la.y - lb.y, la.z - lb.z),
                              glm::vec3(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z),
                              glm::vec3(p2.x - p0.x, p2.y - p0.y, p2.z - p0.z)))
                * glm::vec3(la.x - p0.x, la.y - p0.y, la.z - p0.z);
            float t = tuv.x, u = tuv.y, v = tuv.z;

            // intersection with the plane
            if (t >= 0-1e-6 && t <= 1+1e-6) {
                // intersection with the triangle
                if (u >= 0-1e-6 && u <= 1+1e-6 &&
                    v >= 0-1e-6 && v <= 1+1e-6 &&
                    (u + v) <= 1+1e-6) {
                    return 1;
                }
            }
        }
    }

    return 0;
}


/**
 * Compute a world2camera view matrix to see from portal 'dst', given
 * the original view and the 'src' portal position.
 */
glm::mat4 portal_view(glm::mat4 orig_view, Mesh* src, Mesh* dst)
{
    glm::mat4 mv = orig_view * src->object2world;
    glm::mat4 portal_cam =
        // 3. transformation from source portal to the camera - it's the
        //    first portal's ModelView matrix:
        mv
        // 2. object is front-facing, the camera is facing the other way:
        * glm::rotate(glm::mat4(1.0), glm::radians(180.0f),
                      glm::vec3(0.0,1.0,0.0))
        // 1. go the destination portal; using inverse, because camera
        //    transformations are reversed compared to object
        //    transformations:
        * glm::inverse(dst->object2world);

    return portal_cam;
}

void view21_logic()
{
    glm::mat4 prev_cam = transforms[MODE_CAMERA];

    /* Handle keyboard-based transformations */
    int delta_t = glutGet(GLUT_ELAPSED_TIME) - last_ticks;
    last_ticks = glutGet(GLUT_ELAPSED_TIME);

    // 5 units per second
    float delta_transZ = transZ_direction * delta_t / 1000.0 * 5 * speed_factor;

    float delta_transX = 0, delta_transY = 0, delta_rotY = 0, delta_rotX = 0;
    if (strife) {
        delta_transX = rotY_direction * delta_t / 1000.0 * 3 * speed_factor;
        // 3 units per second
        delta_transY = rotX_direction * delta_t / 1000.0 * 3 * speed_factor;
        // 3 units per second
    } else {
        delta_rotY =  rotY_direction * delta_t / 1000.0 * 120 * speed_factor;
        // 120° per second
        delta_rotX = -rotX_direction * delta_t / 1000.0 * 120 * speed_factor;
        // 120° per second
    }

    if (view_mode == MODE_OBJECT) {
        main_mesh.object2world =
            glm::rotate(main_mesh.object2world,
                        glm::radians(delta_rotY), glm::vec3(0.0, 1.0, 0.0));
        main_mesh.object2world =
            glm::rotate(main_mesh.object2world, glm::radians(delta_rotX),
                        glm::vec3(1.0, 0.0, 0.0));
        main_mesh.object2world =
            glm::translate(main_mesh.object2world,
                           glm::vec3(0.0, 0.0, delta_transZ));
    } else if (view_mode == MODE_CAMERA) {
        // Camera is reverse-facing, so reverse Z translation and X rotation.
        // Plus, the View matrix is the inverse of the camera2world (it's
        // world->camera), so we'll reverse the transformations.
        // Alternatively, imagine that you transform the world,
        // instead of positioning the camera.
        if (strife) {
            transforms[MODE_CAMERA] =
                glm::translate(glm::mat4(1.0),
                               glm::vec3(delta_transX, 0.0, 0.0)) *
                transforms[MODE_CAMERA];
        } else {
            glm::vec3 y_axis_world =
                glm::mat3(transforms[MODE_CAMERA]) * glm::vec3(0.0, 1.0, 0.0);
            transforms[MODE_CAMERA] =
                glm::rotate(glm::mat4(1.0),
                            glm::radians(-delta_rotY), y_axis_world) *
                transforms[MODE_CAMERA];
        }

        if (strife)
            transforms[MODE_CAMERA] =
                glm::translate(glm::mat4(1.0),
                               glm::vec3(0.0, delta_transY, 0.0)) *
                transforms[MODE_CAMERA];
        else
            transforms[MODE_CAMERA] =
                glm::translate(glm::mat4(1.0),
                               glm::vec3(0.0, 0.0, delta_transZ)) *
                transforms[MODE_CAMERA];

        transforms[MODE_CAMERA] =
            glm::rotate(glm::mat4(1.0),
                        glm::radians(delta_rotX),
                        glm::vec3(1.0, 0.0, 0.0)) * transforms[MODE_CAMERA];
    }

    /* Handle portals */
    // Movement of the camera in world view
    for (int i = 0; i < 2; i++) {
        glm::vec4 la = glm::inverse(prev_cam) * glm::vec4(0.0, 0.0, 0.0, 1.0);
        glm::vec4 lb = glm::inverse(transforms[MODE_CAMERA]) *
            glm::vec4(0.0, 0.0, 0.0, 1.0);

        if (portal_intersection(la, lb, &portals[i]))
            transforms[MODE_CAMERA] =
                portal_view(transforms[MODE_CAMERA],
                            &portals[i], &portals[(i+1)%2]);
    }

    // TODO: make handle arcball as generic function
    /* Handle arcball */
    if (view21_cur_mx != view21_last_mx || view21_cur_my != view21_last_my) {
        glm::vec3 va = get_arcball_vector(view21_last_mx, view21_last_my);
        glm::vec3 vb = get_arcball_vector( view21_cur_mx,  view21_cur_my);
        float angle = acos(min(1.0f, glm::dot(va, vb)));
        glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
        glm::mat3 camera2object =
            glm::inverse(glm::mat3(transforms[MODE_CAMERA]) *
                         glm::mat3(main_mesh.object2world));
        glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
        main_mesh.object2world =
            glm::rotate(main_mesh.object2world, angle, axis_in_object_coord);

        view21_last_mx = view21_cur_mx;
        view21_last_my = view21_cur_my;
    }

    // Model
    // Set in onDisplay() - cf. main_mesh.object2world

    // View
    glm::mat4 world2camera = transforms[MODE_CAMERA];

    // Projection
    glm::mat4 camera2screen =
        glm::perspective(fovy, 1.0f*screen_width/screen_height,
                         zNear, 100.0f);

    glUseProgram(view21_portal_program);
    glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(world2camera));
    glUniformMatrix4fv(uniform_p, 1, GL_FALSE, glm::value_ptr(camera2screen));

    glm::mat4 v_inv = glm::inverse(world2camera);
    glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE, glm::value_ptr(v_inv));

    glutPostRedisplay();
}


bool clip_portal(vector<glm::mat4> view_stack,
                 Mesh* outer_portal, rect* scissor);
void draw_portals(vector<glm::mat4> view_stack, int rec, int outer_portal);
// TODO: problem
// void draw_scene(vector<glm::mat4> view_stack, int rec, int outer_portal = -1);
void draw_scene(vector<glm::mat4> view_stack, int rec, int outer_portal);
void fill_screen()
{
    GLfloat vertices[] = {
        -1, -1, 0, 1,
        1, -1, 0, 1,
        -1,  1, 0, 1,
        -1,  1, 0, 1,
        1, -1, 0, 1,
        1,  1, 0, 1,
    };
    GLuint vbo_vertices;
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Apply object's transformation matrix */
    glm::mat4 m =
        glm::inverse(glm::perspective(fovy, 1.0f*screen_width/screen_height,
                                      zNear, 100.0f));
    glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(m));

    // Save current view matrix - useful since we're going to
    // recursively draw the scene from different points of View
    GLuint cur_program;
    GLfloat save_v[16];
    GLfloat save_v_inv[16];
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&cur_program);
    glGetUniformfv(cur_program, uniform_v, save_v);
    glGetUniformfv(cur_program, uniform_v_inv, save_v_inv);

    glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glEnableVertexAttribArray(attr_v_coord);
    glVertexAttribPointer(
        attr_v_coord,  // attribute
        4,                  // number of elements per vertex, here (x,y,z,w)
        GL_FLOAT,           // the type of each element
        GL_FALSE,           // take our values as-is
        0,                  // no extra data between each position
        0                   // offset of first element
                          );

    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/sizeof(vertices[0])/4);

    glDisableVertexAttribArray(attr_v_coord);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &vbo_vertices);

    // Restore view matrix
    glUniformMatrix4fv(uniform_v, 1, GL_FALSE, save_v);
    glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE, save_v_inv);
}

void draw_camera()
{
    GLfloat vertices[] = {
        -1, -1, 0, 1,
        1, -1, 0, 1,
        1, -1, 0, 1,
        -1,  1, 0, 1,
        -1,  1, 0, 1,
        -1, -1, 0, 1,

        -1,  1, 0, 1,
        1, -1, 0, 1,
        1, -1, 0, 1,
        1,  1, 0, 1,
        1,  1, 0, 1,
        -1,  1, 0, 1,
    };
    GLuint vbo_vertices;
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Apply object's transformation matrix */
    glm::mat4 m = glm::inverse(transforms[MODE_CAMERA]);
    glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(m));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glEnableVertexAttribArray(attr_v_coord);
    glVertexAttribPointer(
        attr_v_coord,  // attribute
        4,                  // number of elements per vertex, here (x,y,z,w)
        GL_FLOAT,           // the type of each element
        GL_FALSE,           // take our values as-is
        0,                  // no extra data between each position
        0                   // offset of first element
                          );

    glDrawArrays(GL_LINES, 0, 6);
    glDrawArrays(GL_LINES, 6, 6);

    glDisableVertexAttribArray(attr_v_coord);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &vbo_vertices);
}

void draw_portal_stencil(vector<glm::mat4> view_stack, Mesh* portal)
{
    GLboolean save_color_mask[4];
    GLboolean save_depth_mask;
    glGetBooleanv(GL_COLOR_WRITEMASK, save_color_mask);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &save_depth_mask);

    //bool debug = (view_stack.size() == 4);
    bool debug = false;

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    glStencilFunc(GL_NEVER, 0, 0xFF);
    glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);  // draw 1s on test fail (always)
    // Note: in Mesa 8's software renderer, nothing is drawn on the
    // stencil buffer, looks like a bug; doesn't happen in stencil/cube.cpp.
    // draw stencil pattern
    glClear(GL_STENCIL_BUFFER_BIT);  // needs mask=0xFF
    glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view_stack[0]));
    portal->draw(*portal, attr_v_coord,
                 -1, -1, -1, -1, -1, -1);

    if (debug) {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT);
        glStencilMask(0x00);
        glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
        fill_screen();
        glutSwapBuffers();
        cout << "swap" << endl;
        sleep(1);
        glStencilMask(0xFF);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    }
    for (unsigned int i = 1; i < view_stack.size() - 1; i++) {  // -1 to ignore last view
        // Increment intersection for current portal
        glStencilFunc(GL_EQUAL, 0, 0xFF);
        glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);  // draw 1s on test fail (always)
        glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view_stack[i]));
        portal->draw(*portal, attr_v_coord,
                     -1, -1, -1, -1, -1, -1);

        if (debug) {
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glClear(GL_COLOR_BUFFER_BIT);
            glStencilMask(0x00);
            glStencilFunc(GL_LEQUAL, 1, 0xFF);
            glDisable(GL_STENCIL_TEST);
            portal->draw(*portal, attr_v_coord,
                         -1, -1, -1, -1, -1, -1);
            glEnable(GL_STENCIL_TEST);
            //fill_screen();
            glutSwapBuffers();
            cout << "swap" << endl;
            sleep(1);
            glStencilMask(0xFF);
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        }
        // Decremental outer portal -> only sub-portal intersection remains
        glStencilFunc(GL_NEVER, 0, 0xFF);
        glStencilOp(GL_DECR, GL_KEEP, GL_KEEP);  // draw 1s on test fail (always)
        glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view_stack[i-1]));
        portal->draw(*portal, attr_v_coord,
                     -1, -1, -1, -1, -1, -1);
        if (debug) {
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glClear(GL_COLOR_BUFFER_BIT);
            glStencilMask(0x00);
            glStencilFunc(GL_LEQUAL, 1, 0xFF);
            fill_screen();
            glutSwapBuffers();
            cout << "swap" << endl;
            sleep(1);
            glStencilMask(0xFF);
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        }
    }

    //glColorMask(GL_TRUE, GL_TRUE, GL_FALSE, GL_TRUE);  // blue-ish filter if drawing on white or grey
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    /* Fill 1 or more */
    glStencilFunc(GL_LEQUAL, 1, 0xFF);
    // glutSwapBuffers();
    // cout << "rec=" << rec << ", i=" << i << endl;
    // sleep(1);
    glColorMask(save_color_mask[0], save_color_mask[1], save_color_mask[2], save_color_mask[3]);
    glDepthMask(save_depth_mask);
    glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view_stack.back()));
    // -Ready to draw main scene-
}

/**
 * Draw the active portals contents
 */
void draw_portals(vector<glm::mat4> view_stack, int rec, int outer_portal)
{
    //if (rec >= 2) return;
    // TODO: replace rec with size threshold for MV * portal.bbox ?

    GLboolean save_stencil_test;
    glGetBooleanv(GL_STENCIL_TEST, &save_stencil_test);

    glEnable(GL_STENCIL_TEST);
    glEnable(GL_SCISSOR_TEST);
    for (int i = 0; i < 2; i++) {
        // Important: don't draw outer_portal's outgoing portal - only
        // draw the same portal when displaying a sub-portal (seen from
        // the other portal).
        if (outer_portal == -1 || i == outer_portal) {
            glm::mat4 portal_cam = portal_view(view_stack.back(), &portals[i],
                                               &portals[(i+1)%2]);
            view_stack.push_back(portal_cam);
            // draw_portal_stencil(view_stack, &portals[i]);
            draw_scene(view_stack, rec + 1, i);
            view_stack.pop_back();
            glUniformMatrix4fv(uniform_v, 1, GL_FALSE,
                               glm::value_ptr(view_stack.back()));
            glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE,
                               glm::value_ptr(glm::inverse(view_stack.back())));
            // TODO: write something without lines,
            // I don't have confidence in its interaction with the stencil buffer
            // glLineWidth(1);
        }
    }
    if (!save_stencil_test) {
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_SCISSOR_TEST);
    }

    // Draw portal in the depth buffer so they are not overwritten
    glClear(GL_DEPTH_BUFFER_BIT);

    GLboolean save_color_mask[4];
    GLboolean save_depth_mask;
    glGetBooleanv(GL_COLOR_WRITEMASK, save_color_mask);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &save_depth_mask);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_TRUE);
    for (int i = 0; i < 2; i++)
        portals[i].draw(portals[i], attr_v_coord,
                        -1, -1, -1, -1, -1, -1);
    glColorMask(save_color_mask[0], save_color_mask[1],
                save_color_mask[2], save_color_mask[3]);
    glDepthMask(save_depth_mask);
}

/**
 * Draw a frame around the portal.
 */
void draw_portal_bbox(Mesh* portal)
{
    // 0.05 frame around the portal
    Mesh portal_bbox;
    portal_bbox.vertices.push_back(glm::vec4(-1.00, -1.05, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4(-1.00,  1.05, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4(-1.05, -1.05, 0, 1));

    portal_bbox.vertices.push_back(glm::vec4(-1.05, -1.05, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4(-1.00,  1.05, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4(-1.05,  1.05, 0, 1));

    portal_bbox.vertices.push_back(glm::vec4( 1.05, -1.05, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4( 1.05,  1.05, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4( 1.00, -1.05, 0, 1));

    portal_bbox.vertices.push_back(glm::vec4( 1.00, -1.05, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4( 1.05,  1.05, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4( 1.00,  1.05, 0, 1));

    portal_bbox.vertices.push_back(glm::vec4(-1.00,  1.05, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4(-1.00,  1.00, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4( 1.00,  1.05, 0, 1));

    portal_bbox.vertices.push_back(glm::vec4( 1.00,  1.05, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4(-1.00,  1.00, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4( 1.00,  1.00, 0, 1));

    portal_bbox.vertices.push_back(glm::vec4(-1.00, -1.00, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4(-1.00, -1.05, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4( 1.00, -1.00, 0, 1));

    portal_bbox.vertices.push_back(glm::vec4( 1.00, -1.00, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4(-1.00, -1.05, 0, 1));
    portal_bbox.vertices.push_back(glm::vec4( 1.00, -1.05, 0, 1));

    for (int i = 0; i < portal_bbox.vertices.size(); i++)
        portal_bbox.normals.push_back(glm::vec3(0,0,1));

    portal_bbox.upload();

    /* Apply object's transformation matrix */
    portal_bbox.object2world = portal->object2world;
    portal_bbox.draw(portal_bbox, attr_v_coord,
                     -1, -1, -1, -1, -1, -1);
    portal_bbox.object2world = portal->object2world *
        glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0, 1, 0));
    portal_bbox.draw(portal_bbox, attr_v_coord,
                     -1, -1, -1, -1, -1, -1);
}

bool clip_portal(vector<glm::mat4> view_stack,
                 Mesh* outer_portal, rect* scissor)
{
    scissor->x = 0;
    scissor->y = 0;
    scissor->w = screen_width;
    scissor->h = screen_height;
    for (unsigned int v = 0; v < view_stack.size() - 1; v++) {
        // -1 to ignore last view
        glm::vec4 p[4];
        rect r;
        bool found_negative_w = false;
        for (int pi = 0; pi < 4; pi++) {
            p[pi] = (glm::perspective(fovy, 1.0f*screen_width/screen_height,
                                      zNear, 100.0f)
                     * view_stack[v]
                     * outer_portal->object2world)
                * outer_portal->vertices[pi];
            if (p[pi].w < 0) {
                // TODO: I tried to deal with that case, but it's quite
                // complex because this means the coordinate is projected from
                // the back of the camera, and we should clip to the min or
                // max of the screen.  I'll let the stencil buffer deal with
                // it for now.
                // Possible fix: restrict the portal rectangle using its line
                // intersection with the camera frustum.
                //cout << "w<0" << endl;
                //glDisable(GL_SCISSOR_TEST);
                found_negative_w = true;
            } else {
                p[pi].x /= p[pi].w;
                p[pi].y /= p[pi].w;
            }
        }
        if (found_negative_w)
            continue;

        glm::vec4 min_x, max_x, max_y, min_y;
        min_x = max_x = min_y = max_y = p[0];
        for (int i = 0; i < 4; i++) {
            if (p[i].x < min_x.x) min_x = p[i];
            if (p[i].x > max_x.x) max_x = p[i];
            if (p[i].y < min_y.y) min_y = p[i];
            if (p[i].y > max_y.y) max_y = p[i];
        }

        // (broken) attempt to deal with w < 0
        // if (min_x.w < 0) { min_x.x = max_x.x; max_x.x =  1; }
        // if (max_x.w < 0) { max_x.x = min_x.x; min_x.x = -1; }

        min_x.x = (max(-1.0f, min_x.x) + 1) / 2 * screen_width;
        max_x.x = (min( 1.0f, max_x.x) + 1) / 2 * screen_width;
        min_y.y = (max(-1.0f, min_y.y) + 1) / 2 * screen_height;
        max_y.y = (min( 1.0f, max_y.y) + 1) / 2 * screen_height;

        r.x = min_x.x;
        r.y = min_y.y;
        r.w = max_x.x-min_x.x;
        r.h = max_y.y-min_y.y;

        // intersection with previous rect
        //cout << "+" << scissor->x << "," << scissor->y << ","
        //<< scissor->w << "," << scissor->h << endl;
        //cout << "+" << r.x << "," << r.y << "," << r.w << "," << r.h << endl;
        {
            int r_min_x = max(r.x, scissor->x);
            int r_max_x = min(r.x+r.w, scissor->x+scissor->w);
            scissor->x = r_min_x;
            scissor->w = r_max_x - scissor->x;
            int r_min_y = max(r.y, scissor->y);
            int r_max_y = min(r.y+r.h, scissor->y+scissor->h);
            scissor->y = r_min_y;
            scissor->h = r_max_y - scissor->y;
        }
        //cout << "=" << scissor->x << "," << scissor->y << ","
        //<< scissor->w << "," << scissor->h << endl;
        if (scissor->w <= 0 || scissor->h <= 0) {
            return false;
        }
    }

    //cout << scissor->x << "," << scissor->y << "," << scissor->w
    //<< "," << scissor->h << endl;
    //cout << endl;
    return true;
}

void draw_scene(vector<glm::mat4> view_stack, int rec, int outer_portal = -1)
{
    if (rec >= 5) {
        //draw_portal_stencil(view_stack, &portals[outer_portal]);
        //draw_mesh(&portals[(outer_portal+1)%2]);
        return;
    }
    rect scissor;
    if (outer_portal != -1) {
        // if basic clipping returns an empty rectangle, we can stop here
        if (!clip_portal(view_stack, &portals[outer_portal], &scissor))
            return;
    }

    // Set view matrix
    glUniformMatrix4fv(uniform_v, 1, GL_FALSE,
                       glm::value_ptr(view_stack.back()));
    glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE,
                       glm::value_ptr(glm::inverse(view_stack.back())));

    glClear(GL_DEPTH_BUFFER_BIT);

    // Draw portals contents
    draw_portals(view_stack, rec, outer_portal);

    if (outer_portal != -1) {
        // clip the current view as much as possible, more efficient than
        // using the stencil buffer
        glScissor(scissor.x, scissor.y, scissor.w, scissor.h);

        // draw the current stencil - or actually recreate it if we just
        // drew a sub-portal and hence messed the stencil buffer
        draw_portal_stencil(view_stack, &portals[outer_portal]);
    }

    // Draw portals frames after the stencil buffer is set
    for (int i = 0; i < 2; i++) {
        draw_portal_bbox(&portals[i]);
        //portals[i].draw_bbox();
    }

    /* Draw scene */
    //light_bbox.draw_bbox();
    main_mesh.draw(main_mesh, attr_v_coord,
                     -1, -1, -1, -1, -1, -1);
    ground_mesh.draw(ground_mesh, attr_v_coord,
                     -1, -1, -1, -1, -1, -1);
    //portals[0].draw();

    // Restore view matrix
    view_stack.pop_back();
    if (view_stack.size() > 0) {
        glUniformMatrix4fv(uniform_v, 1, GL_FALSE,
                           glm::value_ptr(view_stack.back()));
        glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE,
                           glm::value_ptr(glm::inverse(view_stack.back())));
    }

    // glutSwapBuffers();
    // cout << "rec=" << rec << endl;
    // sleep(2);
}

void view21_draw()
{
    glClearColor(0.45, 0.45, 0.45, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(view21_portal_program);

    vector<glm::mat4> view_stack;
    view_stack.push_back(transforms[MODE_CAMERA]);

    glViewport(0, 0, screen_width, screen_height);
    draw_scene(view_stack, 1);

    glViewport(2*screen_width/3, 0, screen_width/3, screen_height/3);
    glClear(GL_DEPTH_BUFFER_BIT);
    view_stack.clear();
    view_stack.push_back(glm::lookAt(
                             glm::vec3(0.0,  9.0,-2.0),   // eye
                             glm::vec3(0.0,  0.0,-2.0),   // direction
                             glm::vec3(0.0,  0.0,-1.0))   // up
                         );
    draw_scene(view_stack, 4);
    draw_camera();
}

void view21_portal_Display()
{
    view21_logic();
    view21_draw();
    glutSwapBuffers();
}

int view21_portal_initResources()
{
    main_mesh.load_obj("data/cube2.21.obj", main_mesh);

    for (int i = -GROUND_SIZE/2; i < GROUND_SIZE/2; i++) {
        for (int j = -GROUND_SIZE/2; j < GROUND_SIZE/2; j++) {
            ground_mesh.vertices.push_back(glm::vec4(i,   0.0,  j+1, 1.0));
            ground_mesh.vertices.push_back(glm::vec4(i+1, 0.0,  j+1, 1.0));
            ground_mesh.vertices.push_back(glm::vec4(i,   0.0,  j,   1.0));
            ground_mesh.vertices.push_back(glm::vec4(i,   0.0,  j,   1.0));
            ground_mesh.vertices.push_back(glm::vec4(i+1, 0.0,  j+1, 1.0));
            ground_mesh.vertices.push_back(glm::vec4(i+1, 0.0,  j,   1.0));
            for (int k = 0; k < 6; k++)
                ground_mesh.normals.push_back(glm::vec3(0.0, 1.0, 0.0));
        }
    }

    glm::vec3 light_position = glm::vec3(0.0,  1.0,  2.0);
    light_bbox_mesh.vertices.push_back(glm::vec4(-0.1, -0.1, -0.1, 0.0));
    light_bbox_mesh.vertices.push_back(glm::vec4( 0.1, -0.1, -0.1, 0.0));
    light_bbox_mesh.vertices.push_back(glm::vec4( 0.1,  0.1, -0.1, 0.0));
    light_bbox_mesh.vertices.push_back(glm::vec4(-0.1,  0.1, -0.1, 0.0));
    light_bbox_mesh.vertices.push_back(glm::vec4(-0.1, -0.1,  0.1, 0.0));
    light_bbox_mesh.vertices.push_back(glm::vec4( 0.1, -0.1,  0.1, 0.0));
    light_bbox_mesh.vertices.push_back(glm::vec4( 0.1,  0.1,  0.1, 0.0));
    light_bbox_mesh.vertices.push_back(glm::vec4(-0.1,  0.1,  0.1, 0.0));
    light_bbox_mesh.object2world = glm::translate(glm::mat4(1), light_position);

    create_portal(&portals[0], screen_width, screen_height, zNear, fovy);
    create_portal(&portals[1], screen_width, screen_height, zNear, fovy);

    portals[0].object2world = glm::translate(glm::mat4(1), glm::vec3(0, 1, -2));
    portals[1].object2world = glm::rotate(glm::mat4(1), glm::radians(-90.0f),
                                          glm::vec3(0, 1, 0))
        * glm::translate(glm::mat4(1), glm::vec3(0, 1.2, -2));

    main_mesh.upload();
    ground_mesh.upload();
    light_bbox_mesh.upload();
    portals[0].upload();
    portals[1].upload();

    view21_portal_program = create_program("glsl/phong-shading.21.v.glsl",
                                           "glsl/phong-shading.21.f.glsl");

    const char* attr_name;
    attr_name = "v_coord";
    attr_v_coord = get_attrib(view21_portal_program, attr_name);
    attr_name = "v_normal";
    attr_v_normal = get_attrib(view21_portal_program, attr_name);

    const char* uniform_name;
    uniform_name = "m";
    uniform_m = get_uniform(view21_portal_program, uniform_name);

    uniform_name = "v";
    uniform_v = get_uniform(view21_portal_program, uniform_name);

    uniform_name = "p";
    uniform_p = get_uniform(view21_portal_program, uniform_name);

    uniform_name = "m_3x3_inv_transp";
    uniform_m_3x3_inv_transp = get_uniform(view21_portal_program, uniform_name);

    uniform_name = "v_inv";
    uniform_v_inv = get_uniform(view21_portal_program, uniform_name);
}

void view21onMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        view21_arcball_on = true;
        view21_last_mx = view21_cur_mx = x;
        view21_last_my = view21_cur_my = y;
    } else {
        view21_arcball_on = false;
    }
}

void view21onMotion(int x, int y)
{
    // if left button is pressed
    if (view21_arcball_on) {
        view21_cur_mx = x;
        view21_cur_my = y;
    }
}

void view21onReshape(int width, int height)
{
    screen_width = width;
    screen_height = height;
    glViewport(0, 0, screen_width, screen_height);
    create_portal(&portals[0], screen_width, screen_height, zNear, fovy);
    create_portal(&portals[1], screen_width, screen_height, zNear, fovy);
}

void view21_portal_freeResources()
{
    glDeleteProgram(view21_portal_program);
}
