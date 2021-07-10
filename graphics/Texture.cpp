#include "Texture.h"
#include <GL/glew.h>
#include <iostream>

Texture::Texture(unsigned int _id, int w, int h) : id(_id), width(w), height(h)
{
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}
