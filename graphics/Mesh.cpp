#include "Mesh.h"
#include <GL/glew.h>

Mesh::Mesh(const float* buffer, size_t _vertices, const int* attrs) : vertices(_vertices)
{
	vertex_size = 0;
	for (int i = 0; attrs[i]; i++) {
		vertex_size += attrs[i];
	}
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	//draw
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_size * vertices * sizeof(float), buffer, GL_STATIC_DRAW);

	//attribs
	int offset = 0;
	int size;
	for (int i = 0; attrs[i]; i++) {
		size = attrs[i];
		glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, vertex_size * sizeof(float), (GLvoid*)(offset * sizeof(float)));
		glEnableVertexAttribArray(i);
		offset += size;
	}

	glBindVertexArray(0); 
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Mesh::draw(unsigned int primitive)
{
	glBindVertexArray(vao);
	glDrawArrays(primitive, 0, vertices);
	glBindVertexArray(0);
}


void Mesh::reload(const float* buff, size_t _vertices)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_size * vertices * sizeof(float), buff, GL_STATIC_DRAW);
	vertices = _vertices;
}