#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

GLuint VAO, VBO, IBO, shader, uniformModel, uniformProjection;

// Variables to track the translation
bool direction = true;
float triOffset = 0.0f;
float triMaxoffset = 0.7f;
float triIncrement = 0.01f;

// Variables to track the rotation
float curAngle;

// Variables to track the scaling
bool sizeDirection = true;
float curSize = 0.4;
float maxSize = 0.8f;
float minSize = 0.1f;

// Vertex Shader
static const char* vShader = "										\n\
#version 330														\n\
																	\n\
layout(location = 0) in vec3 pos;									\n\
																	\n\
out vec4 vCol;														\n\
																	\n\
uniform mat4 model;													\n\
uniform mat4 projection;											\n\
																	\n\
void main()															\n\
{																	\n\
	gl_Position = projection * model * vec4(pos, 1.0);				\n\
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);						\n\
}";

// Fragment Shader
static const char* fShader = "										\n\
#version 330														\n\
																	\n\
in vec4 vCol;														\n\
																	\n\
out vec4 colour;													\n\
																	\n\
void main()															\n\
{																	\n\
	colour = vCol;													\n\
}";

void CreateTriangle()
{
	// Define sides of triangle from the vertices (triangle points)
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	// Defines triangle points 
	GLfloat vertices[] = {
		-1.0, -1.0f, 0.0f,
		0.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	// Steps to create the VAO and VBO

	glGenVertexArrays(1, &VAO); // [1.1] Create VAO ID [1.1]
	glBindVertexArray(VAO); // [1.2] Bind the VAO with that ID [1.2]

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO); // [1.3] Create VBO ID inside the VAO [1.3]
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // [1.4] Bind VBA to the GLA_ARRAY_BUFFER [1.4]
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // [1.5] Bind the buffer data (vertices) to the GL_ARRAY_BUFFER VBO [1.5]

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // [1.6] Define the attribute pointer formatting [1.6]
	glEnableVertexAttribArray(0); // [1.7] Enable the attirube pointer [1.7]

	glBindBuffer(GL_ARRAY_BUFFER, 0); // [1.8.1] Unbind the VBO [1.8.1]

	glBindVertexArray(0); // [1.8.2] Unbind the VAO [1.8.2]

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // The IBO should alway be unbinded after the VAO. 
}

void AddShader(GLuint theProgram, const GLchar* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType); // [2.2] Create empty shader [2.2]

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength); // [2.3] Attach shader source code to shaders [2.3]
	glCompileShader(theShader); // [2.4] Compile shaders [2.4]

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiler the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	glAttachShader(theProgram, theShader); // [2.5] Attach shaders to program [2.5]
}

void CompileShaders() 
{
	shader = glCreateProgram(); // [2.1] Create empty program and give shader the ID [2.1]

	if (!shader)
	{
		printf("Error creating shader program\n");
		return;
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader); // [2.6] Link program (creates executables from shaders and links them together) [2.6]
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(shader); // [2.7] Validate program [2.7]
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

	uniformModel = glGetUniformLocation(shader, "model"); //  Grab the location (id) of the model matrix and assign it to uniformModel
	uniformProjection = glGetUniformLocation(shader, "projection"); //  Grab the location (id) of the projection matrix and assign it to uniformProjection

}

int main()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		printf("GLFW Initialisation failed!");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = No Backwards Compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW Window creation failed");
		glfwTerminate();
		return 1;
	}

	// Get Buffer size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialisation failed");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	// Create Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle(); // Creates VAO and VBO and defines data
	CompileShaders(); // Compiles shaders (via adding shaders to shader program)

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)bufferWidth/(GLfloat)bufferHeight, 0.1f, 100.0f); // Creates a perspective projection matrix

	// Loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get and handle user input
		glfwPollEvents();

		// Check the direction of the movement and adjust the offset in that direction
		if (direction)
		{
			triOffset += triIncrement;
		}
		else {
			triOffset -= triIncrement;
		}

		// Change direction if the triangle is at the max offset range. 
		if (abs(triOffset) >= triMaxoffset) 
		{
			direction = !direction;
		}

		curAngle += 0.4f;
		if (curAngle >= 360)
		{
			curAngle -= 360;
		}

		if (direction)
		{
			curSize += 0.002f;
		}
		else 
		{
			curSize -= 0.002f;
		}

		if (curSize >= maxSize || curSize <= minSize)
		{
			sizeDirection = !sizeDirection;
		}

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader); // Specifies which program to use (by id)

		glm::mat4 model; // Creates an identity matrix

		model = glm::translate(model, glm::vec3(triOffset, 0.0f, -2.5f)); // Applies a translation to the model
		//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // Applies a rotation to the model
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f)); // Applies scale to the model

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model)); // Attach the model matrix at the location of the model in the shader
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection)); // Attach the projection matrix at the location of projection in the shader

		glBindVertexArray(VAO); // Binding the VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // Binding the IBO
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0); // Draw the pyramid
		glBindVertexArray(0); // Unbinding the VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // Unbinding the IBO

		glUseProgram(0); // Unassign shader program

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}