#include "Texture.h"
#include "../SOIL/SOIL.h"
#include <iostream>
using namespace std;

// TODO: http://blog.163.com/jackie_howe/blog/static/19949134720122232039986/
Texture::Texture(const char *file)
{
    texture = SOIL_load_image(file, &width, &height, NULL, 0);
    if (texture == NULL) {
        cerr << "SOIL loading error: " << SOIL_last_result() << endl;
        return;
    }

    glGenTextures(1, &tbo_texture);
    glBindTexture(GL_TEXTURE_2D, tbo_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    SOIL_free_image_data(texture);
    glDeleteTextures(1, &tbo_texture);
}

unsigned int Texture::getWidth()
{
    return width;
}

unsigned int Texture::getHeight()
{
    return height;
}

unsigned int Texture::getTbo()
{
    return tbo_texture;
}
