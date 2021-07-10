#pragma once


class Mesh
{
	unsigned int vao;
	unsigned int vbo;
	size_t vertices, vertex_size;
public:
	Mesh(const float* buffer, size_t _verticles, const int* attrs);
	~Mesh();

	void draw(unsigned int primitive);
	void reload(const float* buff, size_t verticies);
};

