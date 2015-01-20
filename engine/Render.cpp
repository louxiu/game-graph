#include "Render.h"

Render::Render(Mesh *mesh, Program *program)
{
    this->mesh = mesh;
    this->program = program;
}

void Render::set_mesh(Mesh *mesh)
{
    this->mesh = mesh;
}

void Render::set_program(Program *program)
{
    this->program = program;
}

void Render::bind()
{
    if (mesh->vbo_vertices != 0) {
        program->set_attrib(mesh->attr_v_name, mesh->vbo_vertices);
    }

    if (mesh->vbo_colors != 0) {
        program->set_attrib(mesh->attr_c_name, mesh->vbo_colors);
    }

    if (mesh->vbo_texture_vertices != 0) {
        program->set_attrib(mesh->attr_tv_name, mesh->vbo_texture_vertices);
    }

    // TODO: other
}

void Render::draw()
{
    program->use();

    bind();

    /* Push each element in buffer_vertices to the vertex shader */
    mesh->render();
}
