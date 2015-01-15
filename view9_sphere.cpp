#include "view9_sphere.h"
#include "Mesh.h"
#include "util.h"
#include "SOIL/SOIL.h"

#include "global.h"
#include <iostream>
using namespace std;

GLuint view9_sphere_program = 0;
GLuint view9_mytexture_id = 0, view9_mytexture_sunlit_id = 0;
GLuint view9_sphere_vbo = -1;
GLint view9_attr_v_coord = -1, view9_attr_v_normal = 1;
GLint view9_uniform_m = -1, view9_uniform_v = -1, view9_uniform_p = -1,
    view9_uniform_m_3x3_inv_transp = -1, view9_uniform_v_inv = -1,
    view9_uniform_mytexture = -1, view9_uniform_mytexture_sunlit = -1,
    view9_uniform_mytexture_ST = -1;

struct demo view9_demos[] = {
    // Textures Spheres
    {"data/Earthmap720x360_grid.jpg",
     "glsl/sphere.9.v.glsl", "glsl/sphere.9.f.glsl"},

    {"data/Earthmap720x360_grid.jpg",
     "glsl/sphere.9.v.glsl", "glsl/sphere_ST.9.f.glsl"},

    // Lighting Textured Surfaces
    {"data/Earthmap720x360_grid.jpg", "glsl/sphere-gouraud.9.v.glsl",
     "glsl/sphere-gouraud.9.f.glsl"},

    // Glossy Textures
    {"data/Land_shallow_topo_alpha_2048.png", "glsl/sphere-gouraud.9.v.glsl",
     "glsl/sphere-gouraud-glossy.9.f.glsl"},
    {"data/Land_shallow_topo_alpha_2048.png", "glsl/sphere-phong.9.v.glsl",
     "glsl/sphere-phong.9.f.glsl"},

    // Transparent Textures
    {"data/Land_shallow_topo_alpha_2048.png", "glsl/sphere.9.v.glsl",
     "glsl/sphere_discard.9.f.glsl"},

    {"data/Land_shallow_topo_alpha_2048.png",
     "glsl/sphere.9.v.glsl", "glsl/sphere.9.f.glsl"},

    {"data/Land_shallow_topo_alpha_2048.png", "glsl/sphere.9.v.glsl",
     "glsl/sphere_oceans.9.f.glsl"},

    // Layers of Textures
    {"data/Earth_lights_lrg.jpg",
     "glsl/sphere-sunlit.9.v.glsl", "glsl/sphere-sunlit.9.f.glsl"},
};
int view9_cur_demo = 0;

int view9_Sphere_initResources();

void view9SphereLogic()
{
    float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 30;  // 30° per second
    glm::mat4 anim = glm::rotate(glm::mat4(1.0f),
                                 glm::radians(angle),
                                 glm::vec3(0, 1, 0));

    // Fix for Blender- or GLUT-style orientation (Z-is-up).
    // Not necessary since switching to our own sphere code, but require
    // fixing 'latitudeLongitude' in the shaders.
    glm::mat4 fix_orientation = glm::rotate(glm::mat4(1.0f),
                                            glm::radians(-90.f),
                                            glm::vec3(1, 0, 0));

    glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                     glm::vec3(0.0, 0.0, -2.0));

    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0),
                                 glm::vec3(0.0, 0.0, -2.0),
                                 glm::vec3(0.0, 1.0, 0.0));

    glm::mat4 projection = glm::perspective(45.0f,
                                            1.0f*128/128,
                                            0.1f, 10.0f);

    //glm::mat4 mvp = projection * view * model * anim * fix_orientation;
    //glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

    glUseProgram(view9_sphere_program);
    glm::mat4 m = model * anim * fix_orientation;
    glUniformMatrix4fv(view9_uniform_m, 1, GL_FALSE, glm::value_ptr(m));

    /* Transform normal vectors with transpose of inverse of upper left
       3x3 model matrix (ex-gl_NormalMatrix): */
    glm::mat3 view9_m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(m)));
    glUniformMatrix3fv(view9_uniform_m_3x3_inv_transp, 1, GL_FALSE,
                       glm::value_ptr(view9_m_3x3_inv_transp));

    glUniformMatrix4fv(view9_uniform_v, 1, GL_FALSE, glm::value_ptr(view));
    glm::mat4 v_inv = glm::inverse(view);
    glUniformMatrix4fv(view9_uniform_v_inv, 1, GL_FALSE, glm::value_ptr(v_inv));

    glUniformMatrix4fv(view9_uniform_p, 1, GL_FALSE, glm::value_ptr(projection));

    // tiling and offset
    if (view9_uniform_mytexture_ST >= 0) {
        glUniform4f(view9_uniform_mytexture_ST, 2, 1, 0, -.05);
    }

}

void view9SphereDisplay()
{
    view9SphereLogic();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(view9_sphere_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, view9_mytexture_id);
    glUniform1i(view9_uniform_mytexture, /*GL_TEXTURE*/0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, view9_mytexture_sunlit_id);
    glUniform1i(view9_uniform_mytexture_sunlit, /*GL_TEXTURE*/1);

    // To be used when FreeGLUT 3.0.0 is out :)
    // glutSetVertexAttribCoord3(attribute_v_coord);
    // glutSetVertexAttribNormal(attribute_v_normal);
    // glutSolidSphere(1.0,30,30);

    glEnableVertexAttribArray(view9_attr_v_coord);
    glEnableVertexAttribArray(view9_attr_v_normal);
    glBindBuffer(GL_ARRAY_BUFFER, view9_sphere_vbo);
    glVertexAttribPointer(view9_attr_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(view9_attr_v_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // TODO: what's this?
    glCullFace(GL_FRONT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * 31 * 30);

    glCullFace(GL_BACK);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * 31 * 30);

    glutSwapBuffers();
}

// TODO: something wrong with 3d texture, no back cube

void view9SphereKeyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case '1':
        {
            view9_cur_demo = ++view9_cur_demo %
                (sizeof(view9_demos)/sizeof(struct demo));

            // printf ("view9_demos = %d\n", sizeof(view9_demos));
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

    if (view9_sphere_program != 0){
        glDeleteProgram(view9_sphere_program);
    }

    if (view9_mytexture_id != 0){
        glDeleteTextures(1, &view9_mytexture_id);
    }

    if (view9_mytexture_sunlit_id != 0){
        glDeleteTextures(1, &view9_mytexture_sunlit_id);
    }

    view9_Sphere_initResources();
}

// TODO: add shadow to subwindow when selected
int view9_Sphere_initResources()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);


    glClearColor(1.0, 1.0, 1.0, 0);
    // printf("init_resources: %s %s %s\n",
    //        view9_demos[view9_cur_demo].texture_filename,
    //        view9_demos[view9_cur_demo].vshader_filename,
    //        view9_demos[view9_cur_demo].fshader_filename);

    view9_mytexture_id = SOIL_load_OGL_texture(
        view9_demos[view9_cur_demo].texture_filename,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);

    if (view9_mytexture_id == 0) {
        cerr << "SOIL loading error: '"
             << SOIL_last_result()
             << "' (" << view9_demos[view9_cur_demo].texture_filename
             << ")" << endl;
    }

    // day-time texture:
    view9_mytexture_sunlit_id = SOIL_load_OGL_texture(
        "data/Land_shallow_topo_2048.jpg",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);

    if (view9_mytexture_sunlit_id == 0) {
        cerr << "SOIL loading error: '"
             << SOIL_last_result()
             << "' ("
             << "Land_shallow_topo_2048.jpg"
             << ")" << endl;
    }

    view9_sphere_program = create_program(
        view9_demos[view9_cur_demo].vshader_filename,
        view9_demos[view9_cur_demo].fshader_filename);

    const char* attr_name;
    attr_name = "v_coord";
    view9_attr_v_coord = glGetAttribLocation(view9_sphere_program, attr_name);
    if (view9_attr_v_coord == -1) {
        fprintf(stderr, "Could not bind attr %s\n", attr_name);
        return 0;
    }

    attr_name = "v_normal";
    view9_attr_v_normal = glGetAttribLocation(view9_sphere_program, attr_name);
    if (view9_attr_v_normal == -1) {
        // fprintf(stderr, "Warning: Could not bind attr %s\n", attr_name);
    }

    const char* uniform_name;
    uniform_name = "m";
    view9_uniform_m = get_uniform(view9_sphere_program, uniform_name);
    if (view9_uniform_m == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
        return 0;
    }

    uniform_name = "v";
    view9_uniform_v = get_uniform(view9_sphere_program, uniform_name);
    if (view9_uniform_v == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
        return 0;
    }

    uniform_name = "p";
    view9_uniform_p = get_uniform(view9_sphere_program, uniform_name);
    if (view9_uniform_p == -1) {
        fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
        return 0;
    }

    uniform_name = "m_3x3_inv_transp";
    view9_uniform_m_3x3_inv_transp = get_uniform(view9_sphere_program,
                                                 uniform_name);

    uniform_name = "v_inv";
    view9_uniform_v_inv = get_uniform(view9_sphere_program,
                                      uniform_name);

    uniform_name = "mytexture";
    view9_uniform_mytexture = get_uniform(view9_sphere_program,
                                          uniform_name);

    uniform_name = "mytexture_sunlit";
    view9_uniform_mytexture_sunlit = get_uniform(view9_sphere_program,
                                                 uniform_name);

    if (strstr(view9_demos[view9_cur_demo].fshader_filename, "_ST")) {
        uniform_name = "mytexture_ST";
        view9_uniform_mytexture_ST = get_uniform(view9_sphere_program,
                                                 uniform_name);
    }

    view9_sphere_vbo = sphere(1, 30, 30);

    return 0;
}

void view9_Sphere_freeResources()
{
    glDeleteProgram(view9_sphere_program);
    glDeleteTextures(1, &view9_mytexture_id);
    glDeleteTextures(1, &view9_mytexture_sunlit_id);
}

void view9_entry(Window *window)
{
    window->display = view9SphereDisplay;
    window->entry = viewEntry;
    window->init = view9_Sphere_initResources;
    window->free = view9_Sphere_freeResources;
    window->keyboard = view9SphereKeyboard;
}

#ifdef TEST_ALONE
int main(int argc, char *argv[])
{
    Window window;
    resetWindow(&window);

    view9_entry(&window);

    if (mini_initWindow(argc, argv, &window) == 0){
        glutMainLoop();
    }

    return 0;
}
#endif /* TEST_ALONE */
