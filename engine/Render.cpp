#include "Render.h"

Render::Render(Mesh *mesh, Program *program)
{
    this->mesh = mesh;
    this->program = program;
    this->totalRenderCalls = 0;
    this->blendingDisabled = false;
    this->blendDstFunc = GL_SRC_ALPHA;
    this->blendSrcFunc = GL_ONE_MINUS_SRC_ALPHA;
    this->cullfaceDisabled = false;
    this->depthTestDisabled = false;
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

void Render::disableDepthTest()
{
    depthTestDisabled = true;
}

void Render::enableDepthTest()
{
    depthTestDisabled = false;
}

void Render::disableBlending()
{
    blendingDisabled = true;
}

void Render::enableBlending()
{
    blendingDisabled = false;
}

void Render::disableCullface()
{
    cullfaceDisabled = true;
}

void Render::enableCullface()
{
    cullfaceDisabled = false;
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

    if (cullfaceDisabled){
        glDisable(GL_CULL_FACE);
    } else {
        glEnable(GL_CULL_FACE);
    }

    if (depthTestDisabled){
        glDisable(GL_DEPTH_TEST);
    } else {
        glEnable(GL_DEPTH_TEST);
    }

    this->totalRenderCalls++;

    // bind();

    /* Push each element in buffer_vertices to the vertex shader */
    mesh->render(*this->program);
}
