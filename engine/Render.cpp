#include "Render.h"

Render::Render(Mesh *mesh, Program *program)
{
    this->mesh = mesh;
    this->program = program;
}

Render::~Render()
{
}

void Render::set_mesh(Mesh *mesh)
{
    this->mesh = mesh;
}

void Render::set_program(Program *program)
{
    this->program = program;
}

void Render::draw()
{
    program->use();

    // bind();

    /* Push each element in buffer_vertices to the vertex shader */
    mesh->render();
}
