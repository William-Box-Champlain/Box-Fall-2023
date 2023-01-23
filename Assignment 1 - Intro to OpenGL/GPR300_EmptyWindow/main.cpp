#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <stdio.h>

//void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void resizeFrameBufferCallback(GLFWwindow* window, int width, int height);

//TODO: Vertex shader source code
const char* vertexShaderSource =
"#version 450							\n"
"layout (location = 0) in vec3 in_Pos;  \n"
"layout (location = 1) in vec4 in_Color;\n"
"out vec4 Color;						\n"
"uniform float iTime;					\n"
"void main(){							\n"
"	Color = in_Color;					\n"
"	vec3 pos = in_Pos;					\n"
"	pos.y += sin(iTime) / 3;			\n"
"	pos.y += sin(pos.x + iTime) / 3;	\n"
"	pos.x += cos(iTime) / 3;			\n"
"	pos.x += cos(pos.y + iTime) / 3;	\n"
"	gl_Position = vec4(pos, 1.0);		\n"
"}										\0";

//TODO: Fragment shader source code
const char* fragmentShaderSource =
"#version 450							\n"
"out vec4 FragColor;					\n"
"in vec4 Color;							\n"
"uniform float iTime;					\n"
"void main(){							\n"
"	FragColor = abs(sin(iTime))*Color;	\n"
"}										\0";

//TODO: Vertex data array
const float coloredVertexData[] = {
	//	x		y		z		r		g		b		a
	   -0.5f,  -0.5f,	0.0f,	0.0f,	0.0f,	1.0f,	1.0f,	//bottom left
		0.0f,  -0.5f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	//bottom right
	   -0.25f,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f,	1.0f,	//top center

	   -0.0f,  -0.5f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	//bottom left
		0.5f,  -0.5f,	0.0f,	1.0f,	0.0f,	0.0f,	1.0f,	//bottom right
		0.25f,	0.0f,	0.0f,	0.0f,	0.0f,	1.0f,	1.0f,	//top center

	   -0.25f,  0.0f,	0.0f,	1.0f,	0.0f,	0.0f,	1.0f,	//bottom left
		0.25f,  0.0f,	0.0f,	0.0f,	0.0f,	1.0f,	1.0f,	//bottom right
		0.0f,	0.5f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	//top center
};

const float vertexData[] = {
	//	x		y		z		
	   -0.5f,  -0.5f,	0.0f,	//bottom left
		0.0f,  -0.5f,	0.0f,	//bottom right
	   -0.25f,	0.0f,	0.0f,	//top center

	   -0.0f,  -0.5f,	0.0f,	//bottom left
		0.5f,  -0.5f,	0.0f,	//bottom right
		0.25f,	0.0f,	0.0f,	//top center

	   -0.25f,  0.0f,	0.0f,	//bottom left
		0.25f,  0.0f,	0.0f,	//bottom right
		0.0f,	0.5f,	0.0f,	//top center
};

int main() {
	if (!glfwInit()) {
		printf("glfw failed to init");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGLExample", 0, 0);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		printf("glew failed to init");
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, resizeFrameBufferCallback);


	//TODO: Create and compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//TODO: Get vertex shader compilation status and output info log
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("Failed to compile vertex shader %s", infoLog);
	}
	
	//TODO: Create and compile fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//TODO: Get fragment shader compilation status and output info log
	success;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("Failed to compile vertex shader %s", infoLog);
	}

	//TODO: Create shader program
	GLuint shaderProgram = glCreateProgram();

	//TODO: Attach vertex and fragment shaders to shader program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	//TODO: Link shader program
	glLinkProgram(shaderProgram);

	//TODO: Check for link status and output errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("Failed to link shader program: %s", infoLog);
		return -1;
	}

	//TODO: Delete vertex + fragment shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader); 

	//TODO: Create and bind Vertex Array Object (VAO)
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	//TODO: Create and bind Vertex Buffer Object (VBO), fill with vertexData
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(coloredVertexData), coloredVertexData, GL_STATIC_DRAW);

	//TODO: Define vertex attribute layout
	//glVertexAttribPointer(...)
	//glEnableVertexAttribArray(...)

	//vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void*)0);
	glEnableVertexAttribArray(0);

	//vertex color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//TODO:Use shader program
		//glUseProgram(...)
		glUseProgram(shaderProgram);
		
		//TODO: Draw triangle (3 indices!)
		//glDrawArrays(...)

		float time = (float)glfwGetTime();

		glProgramUniform1f(shaderProgram, glGetUniformLocation(shaderProgram,"iTime"), time);

		glDrawArrays(GL_TRIANGLES, 0, 9);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void resizeFrameBufferCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

