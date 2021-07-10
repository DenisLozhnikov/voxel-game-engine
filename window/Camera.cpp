#include "Camera.h"
#include "Window.h"

void Camera::updateVecs()
{
    front = vec3(rot * vec4(0, 0, -1, 1)); // -z
    right = vec3(rot * vec4(1, 0, 0, 1));
    up = vec3(rot * vec4(0, 1, 0, 1));
    down = vec3(rot * vec4(0, -1, 0, 1));
}

Camera::Camera(vec3 _pos, float _fov) : pos(_pos), fov(_fov), rot(1.0f)
{
    updateVecs();
}

void Camera::move(float x, float y, float z)
{
    rot = rotate(rot, z, vec3(0, 0, 1));
    rot = rotate(rot, y, vec3(0, 1, 0));
    rot = rotate(rot, x, vec3(1, 0, 0));

    updateVecs();
}

mat4 Camera::getProj()
{
    float aspect = (float)Window::width / (float)Window::height;
    return perspective(fov, aspect, 0.1f, 100.0f);
}

mat4 Camera::getView()
{
    return lookAt(pos, pos + front, up);
}
