#include "view10_cube.h"

#include "Mesh.h"
#include "util.h"
#include "SOIL/SOIL.h"
#include <iostream>
using namespace std;

GLuint view10_cube_program = 0;
GLuint view10_normalmap_id = 0;
GLint view10_attr_v_coord = -1, view10_attr_v_normal = -1;
GLint view10_attr_v_texcoords = -1, view10_attr_v_tangent = 1;
GLint view10_uniform_m = -1, view10_uniform_v = -1, view10_uniform_p = -1,
    view10_uniform_m_3x3_inv_transp = -1, view10_uniform_v_inv = -1,
    view10_uniform_normalmap = -1;

struct demo view10_demos[] = {
    // Lighting of Bumpy Surfaces
    {"data/IntP_Brick_NormalMap.png",
     "glsl/cube.10.v.glsl", "glsl/cube.10.f.glsl"},
};

class Mesh1 {
private:
    GLuint vbo_vertices, vbo_normals, vbo_texcoords, vbo_tangents, ibo_elements;
public:
    vector<glm::vec4> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec2> texcoords;
    vector<glm::vec3> tangents;
    vector<GLushort> elements;
    glm::mat4 object2world;

    Mesh1() : vbo_vertices(0), vbo_normals(0), vbo_texcoords(0), vbo_tangents(0),
              ibo_elements(0), object2world(glm::mat4(1)) {}
    ~Mesh1() {
        if (vbo_vertices != 0)
            glDeleteBuffers(1, &vbo_vertices);
        if (vbo_normals != 0)
            glDeleteBuffers(1, &vbo_normals);
        if (vbo_texcoords != 0)
            glDeleteBuffers(1, &vbo_texcoords);
        if (vbo_tangents != 0)
            glDeleteBuffers(1, &vbo_tangents);
        if (ibo_elements != 0)
            glDeleteBuffers(1, &ibo_elements);
    }

    /**
     * Express surface tangent in object space
     * http://www.terathon.com/code/tangent.html
     * http://www.3dkingdoms.com/weekly/weekly.php?a=37
     */
    void compute_tangents() {
        tangents.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));

        for (int i = 0; i < elements.size(); i+=3) {
            int i1 = elements.at(i);
            int i2 = elements.at(i+1);
            int i3 = elements.at(i+2);
            glm::vec3 p1(vertices.at(i1));
            glm::vec3 p2(vertices.at(i2));
            glm::vec3 p3(vertices.at(i3));
            glm::vec2 uv1 = texcoords.at(i1);
            glm::vec2 uv2 = texcoords.at(i2);
            glm::vec2 uv3 = texcoords.at(i3);

            glm::vec3 p1p2 = p2 - p1;
            glm::vec3 p1p3 = p3 - p1;
            glm::vec2 uv1uv2 = uv2 - uv1;
            glm::vec2 uv1uv3 = uv3 - uv1;

            float c = uv1uv2.s * uv1uv3.t - uv1uv3.s * uv1uv2.t;
            if (c != 0) {
                float mul = 1.0 / c;
                glm::vec3 tangent = (p1p2 * uv1uv3.t - p1p3 * uv1uv2.t) * mul;
                tangents.at(i1) = tangents.at(i2) = tangents.at(i3) = glm::normalize(tangent);
            }
        }
    }

    /**
     * Store object vertices, normals and/or elements in graphic card
     * buffers
     */
    void upload() {
        if (this->vertices.size() > 0) {
            glGenBuffers(1, &this->vbo_vertices);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
            glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(this->vertices[0]),
                         this->vertices.data(), GL_STATIC_DRAW);
        }

        if (this->normals.size() > 0) {
            glGenBuffers(1, &this->vbo_normals);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
            glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(this->normals[0]),
                         this->normals.data(), GL_STATIC_DRAW);
        }

        if (this->texcoords.size() > 0) {
            glGenBuffers(1, &this->vbo_texcoords);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_texcoords);
            glBufferData(GL_ARRAY_BUFFER, this->texcoords.size() * sizeof(this->texcoords[0]),
                         this->texcoords.data(), GL_STATIC_DRAW);
        }

        if (this->tangents.size() > 0) {
            glGenBuffers(1, &this->vbo_tangents);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_tangents);
            glBufferData(GL_ARRAY_BUFFER, this->tangents.size() * sizeof(this->tangents[0]),
                         this->tangents.data(), GL_STATIC_DRAW);
        }

        if (this->elements.size() > 0) {
            glGenBuffers(1, &this->ibo_elements);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->elements.size() * sizeof(this->elements[0]),
                         this->elements.data(), GL_STATIC_DRAW);
        }
    }

    /**
     * Draw the object
     */
    void draw() {
        if (this->vbo_vertices != 0) {
            glEnableVertexAttribArray(view10_attr_v_coord);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
            glVertexAttribPointer(
                view10_attr_v_coord,  // attribute
                4,                  // number of elements per vertex, here (x,y,z,w)
                GL_FLOAT,           // the type of each element
                GL_FALSE,           // take our values as-is
                0,                  // no extra data between each position
                0                   // offset of first element
                                  );
        }

        if (this->vbo_normals != 0) {
            glEnableVertexAttribArray(view10_attr_v_normal);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
            glVertexAttribPointer(
                view10_attr_v_normal,  // attribute
                3,                   // number of elements per vertex, here (x,y,z)
                GL_FLOAT,            // the type of each element
                GL_FALSE,            // take our values as-is
                0,                   // no extra data between each position
                0                    // offset of first element
                                  );
        }

        if (this->vbo_texcoords != 0) {
            glEnableVertexAttribArray(view10_attr_v_texcoords);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_texcoords);
            glVertexAttribPointer(
                view10_attr_v_texcoords, // attribute
                2,                     // number of elements per vertex, here (x,y)
                GL_FLOAT,              // the type of each element
                GL_FALSE,              // take our values as-is
                0,                     // no extra data between each position
                0                      // offset of first element
                                  );
        }

        if (this->vbo_tangents != 0) {
            glEnableVertexAttribArray(view10_attr_v_tangent);
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_tangents);
            glVertexAttribPointer(
                view10_attr_v_tangent, // attribute
                3,                   // number of elements per vertex, here (x,y,z)
                GL_FLOAT,            // the type of each element
                GL_FALSE,            // take our values as-is
                0,                   // no extra data between each position
                0                    // offset of first element
                                  );
        }

        /* Apply object's transformation matrix */
        glUniformMatrix4fv(view10_uniform_m, 1, GL_FALSE, glm::value_ptr(this->object2world));
        /* Transform normal vectors with transpose of inverse of upper left
           3x3 model matrix (ex-gl_NormalMatrix): */
        glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(this->object2world)));
        glUniformMatrix3fv(view10_uniform_m_3x3_inv_transp, 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));

        /* Push each element in buffer_vertices to the vertex shader */
        if (this->ibo_elements != 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
            int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
            glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
        }

        if (this->vbo_tangents != 0)
            glDisableVertexAttribArray(view10_attr_v_tangent);
        if (this->vbo_texcoords != 0)
            glDisableVertexAttribArray(view10_attr_v_texcoords);
        if (this->vbo_normals != 0)
            glDisableVertexAttribArray(view10_attr_v_normal);
        if (this->vbo_vertices != 0)
            glDisableVertexAttribArray(view10_attr_v_coord);
    }
};
Mesh1 view10_cube;
int view10_cur_demo = 0;

void view10CubeLogic()
{
    // 10° per second
    float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * glm::radians(10.0);

    glm::mat4 anim = \
        glm::rotate(glm::mat4(1.0f), angle*3.0f, glm::vec3(1, 0, 0)) *  // X axis
        glm::rotate(glm::mat4(1.0f), angle*2.0f, glm::vec3(0, 1, 0)) *  // Y axis
        glm::rotate(glm::mat4(1.0f), angle*4.0f, glm::vec3(0, 0, 1));   // Z axis

    glm::vec3 object_position = glm::vec3(0.0, 0.0, -2.0);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), object_position);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 4.0, 0.0),
                                 object_position, glm::vec3(0.0, 1.0, 0.0));

    glm::mat4 projection = glm::perspective(45.0f,
                                            1.0f*128/128,
                                            0.1f, 10.0f);

    glUseProgram(view10_cube_program);
    view10_cube.object2world = model * anim;

    glUniformMatrix4fv(view10_uniform_v, 1, GL_FALSE, glm::value_ptr(view));
    glm::mat4 v_inv = glm::inverse(view);
    glUniformMatrix4fv(view10_uniform_v_inv, 1, GL_FALSE, glm::value_ptr(v_inv));
    glUniformMatrix4fv(view10_uniform_p, 1, GL_FALSE, glm::value_ptr(projection));
}

void view10CubeDisplay()
{
    view10CubeLogic();
    // glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glUseProgram(view10_cube_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, view10_normalmap_id);
    glUniform1i(view10_uniform_normalmap, /*GL_TEXTURE*/0);

    view10_cube.draw();

    glutSwapBuffers();
}

int view10_Cube_initResources()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glClearColor(1.0, 1.0, 1.0, 0);

    // printf("init_resources: %s %s %s\n",
    //        view10_demos[view10_cur_demo].texture_filename,
    //        view10_demos[view10_cur_demo].vshader_filename,
    //        view10_demos[view10_cur_demo].fshader_filename);

    // TODO: try to optimize on Android by using ETC1 texture format
    view10_normalmap_id = SOIL_load_OGL_texture(
        view10_demos[view10_cur_demo].texture_filename,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);

    if (view10_normalmap_id == 0) {
        cerr << "SOIL loading error: '"
             << SOIL_last_result()
             << "' ("
             << view10_demos[view10_cur_demo].texture_filename
             << ")" << endl;
    }

    GLint link_ok = GL_FALSE;
    GLuint vs, fs;
    if ((vs = create_shader(view10_demos[view10_cur_demo].vshader_filename,
                            GL_VERTEX_SHADER))   == 0) {
        return 0;
    }

    if ((fs = create_shader(view10_demos[view10_cur_demo].fshader_filename,
                            GL_FRAGMENT_SHADER)) == 0) {
        return 0;
    }

    view10_cube_program = glCreateProgram();
    glAttachShader(view10_cube_program, vs);
    glAttachShader(view10_cube_program, fs);
    glLinkProgram(view10_cube_program);
    glGetProgramiv(view10_cube_program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        fprintf(stderr, "glLinkProgram:");
        printLog(view10_cube_program);
        return 0;
    }

    const char* attr_name;
    attr_name = "v_coord";
    view10_attr_v_coord = glGetAttribLocation(view10_cube_program, attr_name);
    if (view10_attr_v_coord == -1) {
        fprintf(stderr, "Could not bind attr %s\n", attr_name);
        return 0;
    }

    attr_name = "v_normal";
    view10_attr_v_normal = glGetAttribLocation(view10_cube_program, attr_name);
    if (view10_attr_v_normal == -1) {
        fprintf(stderr, "Warning: Could not bind attr %s\n", attr_name);
    }

    attr_name = "v_texcoords";
    view10_attr_v_texcoords = glGetAttribLocation(view10_cube_program, attr_name);
    if (view10_attr_v_texcoords == -1) {
        fprintf(stderr, "Could not bind attr %s\n", attr_name);
        return 0;
    }

    attr_name = "v_tangent";
    view10_attr_v_tangent = glGetAttribLocation(view10_cube_program, attr_name);
    if (view10_attr_v_tangent == -1) {
        fprintf(stderr, "Warning: Could not bind attr %s\n", attr_name);
    }

    const char* uniform_name;
    uniform_name = "m";
    view10_uniform_m = get_uniform(view10_cube_program, uniform_name);

    uniform_name = "v";
    view10_uniform_v = get_uniform(view10_cube_program, uniform_name);

    uniform_name = "p";
    view10_uniform_p = get_uniform(view10_cube_program, uniform_name);

    // TODO: use get list to avoid error
    uniform_name = "m_3x3_inv_transp";
    view10_uniform_m_3x3_inv_transp = get_uniform(view10_cube_program,
                                                  uniform_name);
    uniform_name = "v_inv";
    view10_uniform_v_inv = get_uniform(view10_cube_program,
                                       uniform_name);

    uniform_name = "normalmap";
    view10_uniform_normalmap = get_uniform(view10_cube_program,
                                           uniform_name);

    // Cube

    // front
    view10_cube.vertices.push_back(glm::vec4(-1.0, -1.0,  1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4( 1.0, -1.0,  1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4( 1.0,  1.0,  1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4(-1.0,  1.0,  1.0,  1.0));
    // top
    view10_cube.vertices.push_back(glm::vec4(-1.0,  1.0,  1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4( 1.0,  1.0,  1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4( 1.0,  1.0, -1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4(-1.0,  1.0, -1.0,  1.0));
    // back
    view10_cube.vertices.push_back(glm::vec4( 1.0, -1.0, -1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4(-1.0, -1.0, -1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4(-1.0,  1.0, -1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4( 1.0,  1.0, -1.0,  1.0));
    // bottom
    view10_cube.vertices.push_back(glm::vec4(-1.0, -1.0, -1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4( 1.0, -1.0, -1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4( 1.0, -1.0,  1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4(-1.0, -1.0,  1.0,  1.0));
    // left
    view10_cube.vertices.push_back(glm::vec4(-1.0, -1.0, -1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4(-1.0, -1.0,  1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4(-1.0,  1.0,  1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4(-1.0,  1.0, -1.0,  1.0));
    // right
    view10_cube.vertices.push_back(glm::vec4( 1.0, -1.0,  1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4( 1.0, -1.0, -1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4( 1.0,  1.0, -1.0,  1.0));
    view10_cube.vertices.push_back(glm::vec4( 1.0,  1.0,  1.0,  1.0));

    for (int i = 0; i < 4; i++)
    { view10_cube.normals.push_back(glm::vec3( 0.0,  0.0,  1.0)); }  // front

    for (int i = 0; i < 4; i++)
    { view10_cube.normals.push_back(glm::vec3( 0.0,  1.0,  0.0)); }  // top

    for (int i = 0; i < 4; i++)
    { view10_cube.normals.push_back(glm::vec3( 0.0,  0.0, -1.0)); }  // back

    for (int i = 0; i < 4; i++)
    { view10_cube.normals.push_back(glm::vec3( 0.0, -1.0,  0.0)); }  // bottom

    for (int i = 0; i < 4; i++)
    { view10_cube.normals.push_back(glm::vec3(-1.0,  0.0,  0.0)); }  // left

    for (int i = 0; i < 4; i++)
    { view10_cube.normals.push_back(glm::vec3( 1.0,  0.0,  0.0)); }  // right

    for (int i = 0; i < 6; i++) {
        // front
        view10_cube.texcoords.push_back(glm::vec2(0.0, 0.0));
        view10_cube.texcoords.push_back(glm::vec2(1.0, 0.0));
        view10_cube.texcoords.push_back(glm::vec2(1.0, 1.0));
        view10_cube.texcoords.push_back(glm::vec2(0.0, 1.0));
    }

    // Triangulate
    GLushort view10_cube_elements[] = {
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

    for (int i = 0; i < sizeof(view10_cube_elements)\
             / sizeof(view10_cube_elements[0]); i++) {
        view10_cube.elements.push_back(view10_cube_elements[i]);
    }

    view10_cube.compute_tangents();

    view10_cube.upload();

    return 0;
}

void view10_Cube_freeResources ()
{
    glDeleteProgram(view10_cube_program);
    glDeleteTextures(1, &view10_normalmap_id);
}

void view10_entry(Window *window)
{
    window->program = view10_cube_program;
    window->display = view10CubeDisplay;
    window->entry = viewEntry;
    window->init = view10_Cube_initResources;
    window->free = view10_Cube_freeResources;
}

#ifdef TEST_ALONE
int main(int argc, char *argv[])
{
    Window window;
    resetWindow(&window);

    view10_entry(&window);

    if (mini_initWindow(argc, argv, &window) == 0){
        glutMainLoop();
    }

    return 0;
}
#endif /* TEST_ALONE */
