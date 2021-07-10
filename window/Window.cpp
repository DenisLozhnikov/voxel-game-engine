#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Window.h"

GLFWwindow* Window::window;


int Window::width = 0;
int Window::height = 0;

int Window::init(int width, int height, const char* title)
{
    std::cout << "Creating window...\n";


    if (!glfwInit()) {
        std::cout << "Error on init \n";
        return 0;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW init faliled\n";
        glfwTerminate();
        return -1;
    }

    Window::width = width;
    Window::height = height;
    glViewport(0, 0, width, height);
    return 0;
}

void Window::terminate()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::isShouldClose()
{
    return glfwWindowShouldClose(window);
}

void Window::swapBuffers()
{
    glfwSwapBuffers(window);
}

void Window::setShouldClose(bool flag)
{
    glfwSetWindowShouldClose(window, flag);
}

void Window::setCursorMode(int mode)
{
    glfwSetInputMode(window, GLFW_CURSOR, mode);
}
