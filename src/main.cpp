#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

#include "ECS.hpp"

int s_componentCounter = 0;

struct Transform
{
    glm::vec3 pos;
};

struct Shape
{
    glm::vec3 color;
};

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

    uint32_t extensionCount = 1;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    Scene scene;
    EntityId triangle = scene.newEntity();
    Transform *pTransform = scene.assign<Transform>(triangle);
    Shape *pShape = scene.assign<Shape>(triangle);

    EntityId circle = scene.newEntity();
    scene.assign<Shape>(circle);

    std::cout << "Entity 1 :" << triangle << std::endl;
    std::cout << "Entity 2 :" << circle << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}