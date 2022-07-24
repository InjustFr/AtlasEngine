cmake_minimum_required(VERSION 3.0.0)
project(gameEngine VERSION 0.1.0)

include(CTest)
enable_testing()

file(GLOB_RECURSE project_src "src/*")

add_executable(gameEngine ${project_src})

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 3.3 REQUIRED)

target_include_directories (gameEngine 
    PUBLIC ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(gameEngine ${Vulkan_LIBRARIES})
target_link_libraries(gameEngine glfw)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
