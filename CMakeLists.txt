cmake_minimum_required(VERSION 2.8)

project(hello_triangle)
add_executable(${PROJECT_NAME} "hello_triangle/Source/main.cpp" "hello_triangle/Source/glad.c" "hello_triangle/Source/shader.cpp" "hello_triangle/Source/texture.cpp")

target_link_libraries(${PROJECT_NAME} glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} dl)
