cmake_minimum_required(VERSION 2.8)

project(hello_triangle)
add_executable(${PROJECT_NAME} "main.cpp" "glad.c" "shader.cpp")

target_link_libraries(${PROJECT_NAME} glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} dl)
