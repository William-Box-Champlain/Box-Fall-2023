#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "EW/Shader.h"
#include "EW/ShapeGen.h"
#include "WBox/Transform.h"

void resizeFrameBufferCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);

float lastFrameTime;
float deltaTime;

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

double prevMouseX;
double prevMouseY;
bool firstMouseInput = false;

const int TRANSFORM_COUNT = 5;
WBox::transform boxTransforms[TRANSFORM_COUNT];

WBox::camera sceneCamera;

/* Button to lock / unlock mouse
* 1 = right, 2 = middle
* Mouse will start locked. Unlock it to use UI
* */
const int MOUSE_TOGGLE_BUTTON = 1;
const float MOUSE_SENSITIVITY = 0.1f;

glm::vec3 bgColor = glm::vec3(0);
float exampleSliderFloat = 0.0f;
float rotationSpeed = 0.5f;
float orbitRadius = 5.0f;
float fov = 75.0f;
float orthoHeight = 1.0f;
bool orthoToggle = false;

int main() {
	if (!glfwInit()) {
		printf("glfw failed to init");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Transformations", 0, 0);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		printf("glew failed to init");
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, resizeFrameBufferCallback);
	glfwSetKeyCallback(window, keyboardCallback);

	// Setup UI Platform/Renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Dark UI theme.
	ImGui::StyleColorsDark();

	Shader shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag");

	MeshData cubeMeshData;
	createCube(1.0f, 1.0f, 1.0f, cubeMeshData);

	Mesh cubeMesh(&cubeMeshData);

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Set camera values
	sceneCamera.mPosition = glm::vec3(0, 0, orbitRadius);
	sceneCamera.mWorldUp = glm::vec3(0, 1, 0);
	sceneCamera.mTarget = glm::vec3(0, 0, 0);
	sceneCamera.mFOV = fov;
	sceneCamera.mAspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	sceneCamera.mNearPlane = 0.1f;
	sceneCamera.mFarPlane = 100.0f;
	sceneCamera.mOrthoSize = orthoHeight;
	sceneCamera.mOrthoBool = orthoToggle;

	//Set transform values
	float min = -2.5;
	float max = 2.5;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(min, max);

	for (int i = 0; i < TRANSFORM_COUNT; i++)
	{
		float scalar = (float)dist(gen);

		float xRotation = (float)dist(gen);
		float yRotation = (float)dist(gen);
		float zRotation = (float)dist(gen);

		float xTranslation = (float)dist(gen);
		float yTranslation = (float)dist(gen);
		float zTranslation = (float)dist(gen);

		glm::vec3 randomScale = glm::vec3(scalar);
		glm::vec3 randomRotation = glm::vec3(xRotation, yRotation, zRotation);
		glm::vec3 randomTranslation = glm::vec3(xTranslation, yTranslation, zTranslation);

		boxTransforms[i].updateRotate(randomRotation);
		boxTransforms[i].updateScale(randomScale);
		boxTransforms[i].updateTranslation(randomTranslation);
	}

	while (!glfwWindowShouldClose(window)) {
		glClearColor(bgColor.r,bgColor.g,bgColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float time = (float)glfwGetTime();
		deltaTime = time - lastFrameTime;
		lastFrameTime = time;

		for (int i = 0; i < TRANSFORM_COUNT; i++)
		{
			//update camera values
			sceneCamera.mPosition.x = orbitRadius * cos(rotationSpeed * time);
			sceneCamera.mPosition.z = orbitRadius * sin(rotationSpeed * time);
			sceneCamera.mFOV = fov;
			sceneCamera.mOrthoSize = orthoHeight;
			sceneCamera.mOrthoBool = orthoToggle;

			//Draw
			shader.use();

			glm::mat4 total = glm::mat4(1);
			total = sceneCamera.getProjectionMatrix() * sceneCamera.getViewMatrix() * boxTransforms[i].getTransform();

			shader.setMat4("iTotal",total);

			cubeMesh.draw();
		}

		//Draw UI
		ImGui::Begin("Settings");
		ImGui::SliderFloat("Orbit Speed", &rotationSpeed, 0.0f, 10.0f);
		ImGui::SliderFloat("Orbit Radius", &orbitRadius, 1.0f, 10.0f);
		ImGui::SliderFloat("Camera FOV", &fov, 10.0f, 120.0f);
		ImGui::SliderFloat("Ortho Height", &orthoHeight, 1.0f, 10.0f);
		ImGui::Checkbox("Orthographic Toggle", &orthoToggle);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwPollEvents();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void resizeFrameBufferCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
	if (keycode == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}