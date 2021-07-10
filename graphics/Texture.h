#pragma once
#include <string>

class Texture
{
public:
	unsigned int id;
	int width;
	int height;
	Texture(unsigned int _id, int _width, int _height);
	~Texture();

	void bind(); //bind texture
};

extern Texture* load_texture(std::string fname);

