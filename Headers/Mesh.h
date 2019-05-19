//
// Created by hackyhacker on 18/05/19.
//

#ifndef HELLO_TRIANGLE_MESH_H
#define HELLO_TRIANGLE_MESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Headers/shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

struct Vertex {
    //position
    glm::vec3 Position;
    //normal
    glm::vec3 Normal;
    //texCoords
    glm::vec2 TexCoords;
    //tangent
    glm::vec3 Tangent;
    //bittangent
    glm::vec3 Bittangent;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    unsigned int VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

    void Draw(Shader shader);

private:
    unsigned int VBO, EBO;

    void setupMesh();
};


#endif //HELLO_TRIANGLE_MESH_H
