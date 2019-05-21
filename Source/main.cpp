#include <iostream>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include "../Headers/shader.h"
#include "../Headers/Model.h"
#include "../Headers/Sphere.h"
#include "camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
void renderSphere();
int assignPickingId(int *picking_id, Shader pickingShader);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

Camera camera(glm::vec3(0.0f, 5.0f, 50.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

bool left_button = false;

//picking globals
int pickingId;
bool isSmooth = true;
Shader *currentShader = NULL;
bool isDirectionalLightOn = true;
bool isPointLightOn = true;
bool isFlashLightOn = true;


int main()
{
	ifstream i("../Models/planet.json");
	json j;
	if (i.is_open()) {
		j = json::parse(i);
	}
	string a = j["filename"].get<string>();


	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", glfwGetPrimaryMonitor(), NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// configure global opengl state
	// ------------------------------
	glEnable(GL_DEPTH_TEST);


	//build and compile our shaders
	//Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.fs");
	Shader lightingShader("../Shaders/lighting.vs", "../Shaders/lighting.fs");
	Shader smoothShader("../Shaders/lighting.vs", "../Shaders/lighting.fs");
	Shader flatShader("../Shaders/flat.vs", "../Shaders/flat.fs");
	Shader lampShader("../Shaders/lamp.vs", "../Shaders/lamp.fs");
	Shader pickingShader("../Shaders/lamp.vs", "../Shaders/picking.fs");

	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);

	//Loading our models
	Model rock("../Models/rock/rock.obj");
	Model planet("../Models/planet/planet.obj");

	unsigned int amount = 1000;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime()); // initialize random seed
	float radius = 50.0;
	float offset = 2.5f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}

	int amount2 = 1000;
	glm::mat4* modelMatrices2;
	int modelMatrices2Dist[amount2];
	modelMatrices2 = new glm::mat4[amount];
	srand(glfwGetTime()); // initialize random seed
	for (unsigned int i = 0; i < amount2; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		model = glm::translate(model, glm::vec3((float) (rand() % 100 - 50), (float) (rand() % 100 - 50), -1.0f*i+((float)amount2/2)));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (0.0f);
		//        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices2[i] = model;
		modelMatrices2Dist[i] = (-i+amount2/2);
	}


	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
						// positions          // normals           // texture coords
						-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
						0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
						0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
						0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
						-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
						-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,


						-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
						0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
						0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
						0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
						-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
						-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

						-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
						-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
						-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
						-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
						-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
						-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

						0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
						0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
						0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
						0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
						0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
						0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

						-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
						0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
						0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
						0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
						-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
						-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

						-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
						0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
						0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
						0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
						-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
						-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	// world space positions of our cubes
	glm::vec3 cubePositions[] = {
								 glm::vec3( 0.0f,  0.0f,  0.0f),
								 glm::vec3( 2.0f,  5.0f, -15.0f),
								 glm::vec3(-1.5f, -2.2f, -2.5f),
								 glm::vec3(-3.8f, -2.0f, -12.3f),
								 glm::vec3( 2.4f, -0.4f, -3.5f),
								 glm::vec3(-1.7f,  3.0f, -7.5f),
								 glm::vec3( 1.3f, -2.0f, -2.5f),
								 glm::vec3( 1.5f,  2.0f, -2.5f),
								 glm::vec3( 1.5f,  0.2f, -1.5f),
								 glm::vec3(-1.3f,  1.0f, -1.5f),
								 glm::vec3(-1.3f,  1.0f, 10.5f)
	};

	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
									   glm::vec3(1.2f, 1.0f, 2.0f)
	};

	//cube's vao and vbo
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//lighting vao (vbo stays the same)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// load texture
	// ------------
	unsigned int diffuseMap = loadTexture("../Textures/container.jpg");
	unsigned int specularMap = loadTexture("../Textures/container_specular.jpg");
    unsigned int moonMap = loadTexture("../Textures/moon.jpg");


    lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);


	glShadeModel(GL_FLAT);
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		//lucht is 0
		pickingId = 1;

		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------

		//setting current shader
		if (left_button) {
			currentShader = &pickingShader;
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else if(isSmooth){
			currentShader = &smoothShader;
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		}
		else{
			currentShader = &flatShader;
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		}
		currentShader->use();

		//air color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.setFloat("material.shininess", 32.0f);
		// directional light
		lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		if (isDirectionalLightOn) {
			lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
			lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		}
		else {
			lightingShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("dirLight.diffuse", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);
		}
		// 
		if (isPointLightOn) {
			lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		}
		else {
			lightingShader.setVec3("pointLights[0].ambient", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("pointLights[0].diffuse", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("pointLights[0].specular", 0.0f, 0.0f, 0.0f);
		}
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// spotLight
		lightingShader.setVec3("spotLight.position", camera.Position);
		lightingShader.setVec3("spotLight.direction", camera.Front);
		if(isFlashLightOn) {
			lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		}
		else {
			lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
		}
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09);
		lightingShader.setFloat("spotLight.quadratic", 0.032);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		currentShader->setVec3("viewPos", camera.Position);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);




		//draw sun
        Sphere sun = Sphere();
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(10.0f,10.0f,10.0f));
        int sunId = assignPickingId(&pickingId, pickingShader);
        if (currentShader != &pickingShader) {
            sun.Draw(lampShader, model, projection, view);
        }
        else {
            sun.Draw(pickingShader, model, projection, view);
        }

        // draw planet
        model = glm::mat4(1.0f);
        model = glm::rotate(model, ((float)glfwGetTime()*0.2f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(-20.0f, 0.0f, 0.0f));
        model  = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

		planet.Draw(*currentShader, model, projection, view);



		// draw static meteorites
		for (unsigned int i = 0; i < amount; i++)
		{
			glm::mat4 modelTemp = modelMatrices[i];
			modelMatrices[i] = modelTemp;

			assignPickingId(&pickingId, pickingShader);
			rock.Draw(*currentShader, modelMatrices[i], projection, view);
		}

		// draw flying meteorites
		for (unsigned int i = 0; i < amount2; i++)
		{
			glm::mat4 modelTemp = modelMatrices2[i];
			modelTemp = glm::translate(modelTemp, glm::vec3(0.0f, 0.0f, 1.0f));
			int dist = modelMatrices2Dist[i];
			dist += 1;

			if (dist > amount2/2){
				for (int j = 0; j < amount2; j++){
					modelTemp = glm::translate(modelTemp, glm::vec3(0.0f, 0.0f, -1.0f));
				}
				dist = -amount2/2;
			}
			modelMatrices2Dist[i] = dist;
			modelMatrices2[i] = modelTemp;
			rock.Draw(*currentShader, modelMatrices2[i], projection, view);
		}

		Sphere moon = Sphere();
		//moon
		model = glm::mat4(1.0f);
        model = glm::rotate(model, ((float)glfwGetTime()*0.2f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(-20.0f, 0.0f, 0.0f));
        model = glm::rotate(model, ((float)glfwGetTime()*1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(2.0f, -2.0f, 0.0f));
        model  = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

		assignPickingId(&pickingId, pickingShader);
		if (currentShader != &pickingShader) {
			moon.Draw(lampShader, model, projection, view);
		}
		else {
			moon.Draw(*currentShader, model, projection, view);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		if (!left_button) {
			glfwSwapBuffers(window);
		}
		glfwPollEvents();

		if (left_button) { //left button has been pressed
			glFlush();
			glFinish();

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			// Read the pixel at the center of the screen.
			// You can also use glfwGetMousePos().
			// Ultra-mega-over slow too, even for 1 pixel,
			// because the framebuffer is on the GPU.
			unsigned char data[4];
			glReadPixels(1366/2, 768/2,1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);

			int pickedID =
				data[0] +
				data[1] * 256 +
				data[2] * 256*256;

			cout << "Picked id: " << pickedID << endl;

			//if clicked something
			if (pickedID < pickingId) {
				sunId = 1;
				if (pickedID == sunId) {
					if (isPointLightOn) {
						isPointLightOn = false;
					}
					else {
						isPointLightOn = true;
					}
				}
				left_button = false;
			}
		}
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	//glDeleteVertexArrays(1, &lightVAO);
	//glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void printGlmVec3(glm::vec3 vec)
{
	std::cout <<
		std::to_string(vec.x) <<
		std::to_string(vec.y) <<
		std::to_string(vec.z) <<
		std::endl;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	//W op azerty
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	//X op azerty
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	//C op azerty
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		isSmooth = false;
	}
	//V op azerty
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		isSmooth = true;
	}
	//U op azerty
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		isDirectionalLightOn = true;
	}
	//J op azerty
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		isDirectionalLightOn = false;
	}
	//I op azerty
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		isPointLightOn = true;
	}
	//K op azerty
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		isPointLightOn = false;
	}
	//O op azerty
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		isFlashLightOn = true;
	}
	//L op azerty
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		isFlashLightOn = false;
	}

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (left_button) {
			left_button = false;
		}
		else {
			left_button = true;
		}
	}
}


//glfw: whenever the mouse move, this calback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

int assignPickingId(int *picking_id, Shader pickingShader)
{
	int r = ((*picking_id) & 0x000000FF) >>  0;
	int g = ((*picking_id) & 0x0000FF00) >>  8;
	int b = ((*picking_id) & 0x00FF0000) >> 16;

	pickingShader.setVec4("PickingColor", glm::vec4(r/255.0f, g/255.0f, b/255.0f, 1.0f));

	(*picking_id)++;

	return (*picking_id)-1;
}

