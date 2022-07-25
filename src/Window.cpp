#include "Window.hpp"

#include <iostream>

Window::Window()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

    uint32_t extensionCount = 1;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";
}

void Window::run()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
}

Window::~Window()
{
    glfwDestroyWindow(window);

    glfwTerminate();
}