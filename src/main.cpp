#include "ECS.hpp"
#include "Window.hpp"

int s_componentCounter = 0;

int main()
{
    Window window = Window();

    window.run();

    return 0;
}