#pragma once
#include <string>
//#include "../graphics/Texture.h"

class Texture;
extern int png_load(const char* file, int* width, int* height);
extern Texture* load_texture(std::string fn);
