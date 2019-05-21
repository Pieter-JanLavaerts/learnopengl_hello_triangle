#include "../Headers/Sphere.h"

void Sphere::calculateVertices()
{
	for (int i = 0; i <= Stacks; ++i){
		float V   = i / (float) Stacks;
		float phi = V * glm::pi <float> ();

		// Loop Through Slices
		for (int j = 0; j <= Slices; ++j){

			float U = j / (float) Slices;
			float theta = U * (glm::pi <float> () * 2);

			// Calc The Vertex Positions
			float x = cosf (theta) * sinf (phi);
			float y = cosf (phi);
			float z = sinf (theta) * sinf (phi);

			// Push Back Vertex Data
			vertices.push_back (glm::vec3(x, y, z) * Radius);
		}
	}
}

void Sphere::calculateIndices()
{
	for (int i = 0; i < Slices * Stacks + Slices; ++i){
		indices.push_back (i);
		indices.push_back (i + Slices + 1);
		indices.push_back (i + Slices);

		indices.push_back (i + Slices + 1);
		indices.push_back (i);
		indices.push_back (i + 1);
	}
}

void Sphere::generateVAO()
{
	//vao
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//vbo
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0].x, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	//ibo
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

Sphere::Sphere(int stacks, int slices)
{
	Stacks = stacks;
	Slices = slices;

	calculateVertices();
	calculateIndices();
	generateVAO();
}

void Sphere::Draw(Shader shader, glm::mat4 model, glm::mat4 projection, glm::mat4 view)
{
	//setting up the shader
	shader.use();
	shader.setMat4("projection", projection);
	shader.setMat4("view",  view);
	shader.setMat4("model", model);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
