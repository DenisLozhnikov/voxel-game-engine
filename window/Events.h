#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
class Events
{
public:
	static bool* _keys;
	static unsigned int* _frames;
	static unsigned int _current;
	static double dx;
	static double dy;
	static double x;
	static double y;
	static bool _cursor_locked;
	static bool _cursor_released;

	static int init();
	static void refresh();

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void change_size_callback(GLFWwindow* window, int width, int height);

	static bool isPress(int keycode);
	static bool isjPress(int keycode);
	static bool isClick(int button);
	static bool isjClick(int button);
	static void toogleCursor();
};

