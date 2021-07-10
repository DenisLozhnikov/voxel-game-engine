#include "Events.h"

#include "Window.h"
#include <string.h>


#define _MOUSE_OFFSET 1024

bool* Events::_keys;
unsigned int* Events::_frames;
unsigned int Events::_current = 0;
double Events::dx = 0.0f;
double Events::dy = 0.0f;
double Events::x = 0.0f;
double Events::y = 0.0f;

bool Events::_cursor_locked = false;
bool Events::_cursor_released = false;

int Events::init()
{
	GLFWwindow* window = Window::window;
	_keys = new bool[1032];
	_frames = new unsigned int[1032];

	memset(_keys, false, 1032 * sizeof(bool));
	memset(_frames, 0, 1023 * sizeof(unsigned int));

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetWindowSizeCallback(window, change_size_callback);
	return 0;
}

void Events::refresh()
{
	_current++;
	dx = 0.0f;
	dy = 0.0f;

	glfwPollEvents();
}

void Events::change_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Window::width = width;
	Window::height = height;
}

void Events::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS) 
		Events::_keys[key] = true;
	else if (action == GLFW_RELEASE)
		Events::_keys[key] = false;

	Events::_frames[key] = Events::_current;
}

void Events::mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
	if (action == GLFW_PRESS)
		Events::_keys[_MOUSE_OFFSET + button] = true;
	else if (action == GLFW_RELEASE)
		Events::_keys[_MOUSE_OFFSET + button] = false;

	Events::_frames[_MOUSE_OFFSET + button] = Events::_current;
}

void Events::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (Events:: _cursor_released) {
		Events::dx += xpos - Events::x;
		Events::dy += ypos - Events::y;
	}
	else
		Events::_cursor_released = true;
	Events::x = xpos;
	Events::y = ypos;
}

void Events::toogleCursor()
{
	_cursor_locked = !_cursor_locked;

	Window::setCursorMode(_cursor_locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

bool Events::isPress(int keycode)
{
	if (keycode < 0 || keycode >= _MOUSE_OFFSET)
		return false;
	return _keys[keycode];
}

bool Events::isjPress(int keycode) {
	if (keycode < 0 || keycode >= _MOUSE_OFFSET)
		return false;
	return _keys[keycode] && _frames[keycode] == _current;
}

bool Events::isClick(int button)
{
	return _keys[_MOUSE_OFFSET + button];
}

bool Events::isjClick(int button) {
	return _keys[_MOUSE_OFFSET + button] && _frames[_MOUSE_OFFSET + button] == _current;
}