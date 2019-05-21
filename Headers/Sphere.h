#pragma once

#include <vector>

#include <glm/glm.hpp>
#include "shader.h"

#define space 10
#define VERTEX_COUNT (180/space)*(360/space)*2 

const double PI=3.1415926535897;

class Sphere
{
public:
	Sphere(int stacks, int slices);
	void Draw(Shader shader, glm::mat4 projection, glm::mat4 view, glm::mat4 model);

private:
	unsigned int VAO;
	void calculateVertices();
	void calculateIndices();
	void generateVAO();
	int Stacks;
	int Slices;
	const float Radius = 1;
	std::vector<glm::vec3> vertices;
	std::vector<int> indices;
};
