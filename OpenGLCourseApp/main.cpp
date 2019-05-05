#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader;

// Vertex Shader
static const char* vShader = "									\n\
#version 330													\n\
																\n\
layout(location = 0) in vec3 pos;								\n\
																\n\
void main()														\n\
{																\n\
	gl_Position = vec4(0.4 * pos.x, 0.4 * pos.y, pos.z, 1.0);	\n\
}";

// Fragment Shader
static const char* fShader = "									\n\
#version 330													\n\
																\n\
out vec4 colour;												\n\
																\n\
void main()														\n\
{																\n\
	colour = vec4(1.0, 0.0, 0.0, 1.0);							\n\
}";

void CreateTriangle()
{
	// Defines triangle points 
	GLfloat vertices[] = {
		-1.0, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	// Steps to create the VAO and VBO

	glGenVertexArrays(1, &VAO); // [1.1] Create VAO ID [1.1]
	glBindVertexArray(VAO); // [1.2] Bind the VAO with that ID [1.2]

	glGenBuffers(1, &VBO); // [1.3] Create VBO ID inside the VAO [1.3]
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // [1.4] Bind VBA to the GLA_ARRAY_BUFFER [1.4]
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // [1.5] Bind the buffer data (vertices) to the GL_ARRAY_BUFFER VBO [1.5]

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // [1.6] Define the attribute pointer formatting [1.6]
	glEnableVertexAttribArray(0); // [1.7] Enable the attirube pointer [1.7]

	glBindBuffer(GL_ARRAY_BUFFER, 0); // [1.8.1] Unbind the VBO [1.8.1]

	glBindVertexArray(0); // [1.8.2] Unbind the VAO [1.8.2]
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

	// Setup Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle(); // Creates VAO and VBO and defines data
	CompileShaders(); // Compiles shaders (via adding shaders to shader program)

	// Loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get and Handle user input event
		glfwPollEvents();

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader); // Specifies which program to use (by ID)

		glBindVertexArray(VAO); // Binding the VAO
		glDrawArrays(GL_TRIANGLES, 0, 3); // Draw the object to the window
		glBindVertexArray(0); // Unbinding the VAO

		glUseProgram(0); // Unassign shader program

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}