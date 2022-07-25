#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window
{
private:
    GLFWwindow *window;

public:
    Window();
    void run();
    ~Window();
};