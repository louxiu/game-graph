#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <GL/glew.h>

class Texture {
  private:
    int width;
    int height;
    unsigned char *texture;
    GLuint tbo_texture;

  public:
    Texture(const char *file);
    virtual ~Texture();

    unsigned int getWidth ();
    unsigned int getHeight ();
    unsigned int getTbo();
};
#endif /* TEXTURE_H_ */
