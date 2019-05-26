#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void CreateObjects()
{
	// Define sides of triangle from the vertices (triangle points)
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	// Define triangle points 
	GLfloat vertices[] = {
		-1.0, -1.0f, 0.0f,
		0.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	// Create new mesh object
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1);

	// Create new mesh object
	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj2);

}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{

	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CreateObjects(); // Creates VAO and VBO and defines data
	CreateShaders(); // Compiles shaders (via adding shaders to shader program)

	GLuint uniformProjection = 0, uniformModel = 0;
	glm::mat4 projection = glm::perspective(45.0f, mainWindow.getBufferWidth()/ mainWindow.getBufferHeight(), 0.1f, 100.0f); // Creates a perspective projection matrix

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		// Get and handle user input
		glfwPollEvents();

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].UseShader(); // Specifies which program to use (by id)
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();

		glm::mat4 model; // Creates an identity matrix

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f)); // Applies a translation to the model
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f)); // Applies scale to the model
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model)); // Attach the model matrix at the location of the model in the shader
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection)); // Attach the projection matrix at the location of projection in the shader
		meshList[0]->RenderMesh(); // Render the first mesh in meshList

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f)); // Applies a translation to the model
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f)); // Applies scale to the model
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model)); // Attach the model matrix at the location of the model in the shader
		meshList[1]->RenderMesh(); // Render the second mesh in meshList

		glUseProgram(0); // Unassign shader program

		mainWindow.swapBuffers();
	}

	return 0;
}