#include "Render.h"

Render::Render(Mesh *mesh, Program *program)
{
    this->mesh = mesh;
    this->program = program;
    this->totalRenderCalls = 0;
    this->blendingDisabled = false;
    this->blendDstFunc = GL_SRC_ALPHA;
    this->blendSrcFunc = GL_ONE_MINUS_SRC_ALPHA;
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

void Render::begin()
{
    // setupMatrices();
    program->begin();
}

void Render::end()
{
    program->end();
}

void Render::disableBlending()
{
    blendingDisabled = true;
}

void Render::enableBlending()
{
    blendingDisabled = false;
}

void Render::setBlendFunction(int srcFunc, int dstFunc)
{
    if (blendSrcFunc == srcFunc && blendDstFunc == dstFunc) return;
    blendSrcFunc = srcFunc;
    blendDstFunc = dstFunc;
}
void Render::draw()
{
    if (blendingDisabled){
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
        glBlendFunc(blendSrcFunc, blendDstFunc);
    }

    this->totalRenderCalls++;

    // bind();

    /* Push each element in buffer_vertices to the vertex shader */
    mesh->render(*this->program);
}
