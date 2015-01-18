#ifndef RENDER_H_
#define RENDER_H_

#include "Render.h"
#include "Mesh.h"
#include "Program.h"

class Render {
  private:
    Mesh *mesh;
    Program *program;
    void bind();

  public:
    Render(Mesh *mesh, Program *program);
    void set_mesh(Mesh *mesh);
    void set_program(Program *program);
    void draw();
};

#endif /* RENDER_H_ */