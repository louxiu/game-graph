#ifndef RENDER_H_
#define RENDER_H_

#include "Render.h"
#include "Mesh.h"
#include "Program.h"

class Render {
  private:
    Mesh *mesh;
    Program *program;
    int totalRenderCalls;
    bool blendingDisabled;
    int blendSrcFunc, blendDstFunc;
    bool cullfaceDisabled;

  public:
    Render(Mesh *mesh, Program *program);
    virtual ~Render();
    void set_mesh(Mesh *mesh);
    void set_program(Program *program);
    void disableBlending();
    void enableBlending();
    void setBlendFunction(int srcFunc, int dstFunc);

    void disableCullface();
    void enableCullface();

    void begin();
    void end();

    void draw();
};

#endif /* RENDER_H_ */
