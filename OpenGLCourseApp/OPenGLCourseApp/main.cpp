#include <iostream>
#include <string.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader;

// Vertex shader
static const char* vShader = "                          \n\
#version 330                                            \n\
														\n\
layout (location = 0) in vec3 pos;                      \n\
														\n\
void main()                                             \n\
{                                                       \n\
	gl_Position = vec4(0.4 * pos.x, 0.4 * pos.y, pos.z, 1.0);       \n\
}";

static const char* fShader = "                          \n\
#version 330                                            \n\
														\n\
out vec4 color;					                        \n\
														\n\
void main()                                             \n\
{                                                       \n\
	color = vec4(1.0, 0.0, 0.0, 1.0);       \n\
}";

void CreateTriangle()
{	
	// Aka vertex specification
	GLfloat vertices[] = {
		-1.0f,  -1.0f, 0.0f,
	   1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO); // create VAO (1 array, where to store id)
	glBindVertexArray(VAO); // bind to this VAO
	// inside VAO we can create multiple VBOs
		glGenBuffers(1, &VBO); // create VBO (1 buffer, where to store id)	
		glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind to this VBO. first arg is type of buffer (vertex buffer) 
		// inside this VBO we can store data
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy vertex data to buffer (type, size, data, usage) from CPU to GPU
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // how OpenGL should interpret vertex data (layout location, size, type, normalized?, stride, offset) (describes)
			glEnableVertexAttribArray(0); // enable vertex attribute at location 0 (activates)
		glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO 
	glBindVertexArray(0);	
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
	GLuint theShader = glCreateShader(shaderType); // create shader object for type
	const GLchar* theCode[1]; // array of pointers to code
	theCode[0] = shaderCode; // pointer to code
	GLint codeLength[1]; // array of code lengths
	codeLength[0] = strlen(shaderCode); // length of code
	glShaderSource(theShader, 1, theCode, codeLength); // attach code to shader object (shader object, number of strings, array of pointers to code, array of code lengths)
	glCompileShader(theShader); 

	// error checking
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);
}

void CompileShaders() {
	shader = glCreateProgram();
	if (!shader) {
		printf("Shader creation failed");
		return;
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	// error checking
	GLint result = 0;
	GLchar eLog[1024] = { 0 }; // error log buffer

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

}

int main()
{
	// initialize GLFW
	if (!glfwInit()) {
		printf("GLFW initialization failed");
		glfwTerminate();
		return 1;
	}

	// setup GLFW window properties
	// OpenGL version

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // makes it not backwards compatible
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);

	if (!mainWindow) {
		printf("GLFW window creation failed");
		glfwTerminate();
		return 1;
	}

	// get buffer size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// allow modern extension features
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		printf("GLEW initialization failed");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShaders();	


	// loop until window closed
	while (!glfwWindowShouldClose(mainWindow)) {
		// get + handle user input events
		glfwPollEvents();
		// clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);
		glUseProgram(0);

		// swap buffers
		glfwSwapBuffers(mainWindow);
	}
	return 0;
}
