#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace glm;

class Camera
{
	void updateVecs();
public:
	vec3 front, up, right, down;	//camera edges
	float fov;
	vec3 pos;
	mat4 rot;	//rotation

	Camera(vec3 pos, float fov);

	void move(float x, float y, float z);
	mat4 getProj();
	mat4 getView();
};

