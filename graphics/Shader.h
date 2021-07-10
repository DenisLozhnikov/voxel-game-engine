#pragma once
#include <string>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	unsigned int id;
	Shader(unsigned int _id);
	~Shader();

	void use();
	void uniform_matrix(std::string name, glm::mat4 matrix);
};

extern Shader* load_shader(std::string vFile, std::string fFile);

