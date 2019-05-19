cmake_minimum_required(VERSION 2.8)

project(hello_triangle)
add_executable(${PROJECT_NAME}
    Source/main.cpp
    Source/glad.c
    Source/shader.cpp
    Source/camera.cpp
    Source/camera.h
    Source/Mesh.cpp
    Headers/Mesh.h
    Source/Model.cpp
    Headers/Model.h
    Headers/stb_image.h
    Headers/Sphere.h
    Source/Sphere.cpp)

add_definitions("-fPIC")

target_link_libraries(${PROJECT_NAME} glfw assimp
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} dl)
