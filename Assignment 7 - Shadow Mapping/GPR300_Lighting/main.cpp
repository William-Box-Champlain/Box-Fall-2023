#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "EW/Shader.h"
#include "EW/EwMath.h"
#include "EW/Camera.h"
#include "EW/Mesh.h"
#include "EW/Transform.h"
#include "EW/ShapeGen.h"

#include "WB/LightStructs.h"

struct DepthMap
{
	GLuint mFBO;
	GLuint mDepthMap;
};

GLuint createTexture(std::string filepath);
void createColorBuffer(GLuint &colorBuffer);
GLuint createDepthBuffer();
DepthMap createDepthMap(glm::vec2 shadowResolution);

void processInput(GLFWwindow* window);
void resizeFrameBufferCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void updateLights(float time, ew::Transform lightTransform, ew::Transform pointLightTransform[]);
void drawObjects(Shader& shader,int numOfObjects, ew::Mesh meshes[], ew::Transform transforms[]);
void drawObject(Shader& shader, ew::Mesh& mesh, ew::Transform& transform);
void processAllLights(Shader& litShader);
//void drawUI(Material material, DirLight directionalLight, SpotLight spotLight, ew::Transform lightTransform, glm::vec3 orbitalCenter, int effectIndex, glm::vec2 effectIntensity);

float lastFrameTime;
float deltaTime;

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

double prevMouseX;
double prevMouseY;
bool firstMouseInput = false;

/* Button to lock / unlock mouse
* 1 = right, 2 = middle
* Mouse will start locked. Unlock it to use UI
* */
const int MOUSE_TOGGLE_BUTTON = 1;
const float MOUSE_SENSITIVITY = 0.1f;
const float CAMERA_MOVE_SPEED = 5.0f;
const float CAMERA_ZOOM_SPEED = 3.0f;

Camera camera((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);

glm::vec3 bgColor = glm::vec3(0);
glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPosition = glm::vec3(0.0f, 3.0f, 0.0f);

bool wireFrame = false;

Material material;

DirLight directionalLight;

float spotLightMinAngleDegrees = float(23.0f);
float spotLightMaxAngleDegrees = float(25.0f);

SpotLight spotLight =
{
	spotLight.mColor = glm::vec3(128, 128, 128),
	spotLight.mPosition = glm::vec3(0.0f),
	spotLight.mDirection = glm::vec3(0.0f, -1.0f, 0.0f),
	spotLight.mIntensity = float(1.0f),
	spotLight.mRadius = float(12.0f),
	spotLight.mMinAngle = glm::cos(glm::radians(spotLightMinAngleDegrees)),
	spotLight.mMaxAngle = glm::cos(glm::radians(spotLightMaxAngleDegrees)) 
};

float pointLightZeroDistance = 1.0f;
glm::vec3 pointLightDirection = glm::vec3(0.0f);

glm::vec3 orbitalCenter = glm::vec3(0.0f, 5.0f, 0.0f);
float orbitalRadius = 5;
float orbitalSpeed = 1;

float pointLightRadius = 5.0f;

static const int NUMBER_OF_POINTLIGHTS = 3;
PointLight pointLights[NUMBER_OF_POINTLIGHTS];

glm::vec2 SHADOW_RESOLUTION = glm::vec2(1080, 720);

std::string CORRUGATED_STEEL_TEXTURE_FILE_NAME = "textures/CorrugatedSteel/CorrugatedSteel007C_1K_Color.jpg";
std::string CORRUGATED_STEEL_NORMAL_MAP = "textures/CorrugatedSteel/CorrugatedSteel007C_1K_NormalGL.jpg";
std::string PAVING_STONES_TEXTURE_FILE_NAME = "textures/PavingStones/PavingStones128_1K_Color.jpg";
std::string PAVING_STONES_NORMAL_MAP = "textures/PavingStones/PavingStones128_1K_NormalGL.jpg";
std::string NOISE_TEXTURE_FILE_NAME = "textures/noiseTexture.png";

float sampleSize = 0.01f;

const char* effectNames[4] = {
	"None", "Gaussian Blur", "Sharpen", "Edge Detection"
};
int effectIndex = 0;

int main() {
	if (!glfwInit()) {
		printf("glfw failed to init");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lighting", 0, 0);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		printf("glew failed to init");
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, resizeFrameBufferCallback);
	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	//Hide cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Setup UI Platform/Renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Dark UI theme.
	ImGui::StyleColorsDark();

	//Create Textures
	//GLuint Texture = createTexture(CORRUGATED_STEEL_TEXTURE_FILE_NAME);
	//GLuint NormalMap = createTexture(CORRUGATED_STEEL_NORMAL_MAP);
	GLuint Texture = createTexture(PAVING_STONES_TEXTURE_FILE_NAME);
	GLuint NormalMap = createTexture(PAVING_STONES_NORMAL_MAP);
	GLuint noiseTexture = createTexture(NOISE_TEXTURE_FILE_NAME);

	
	//Used to draw shapes. This is the shader you will be completing.
	Shader litShader("shaders/defaultLit.vert", "shaders/defaultLit.frag");

	//Used for Shadow-Mapping
	Shader shadowShader("shaders/shadowMapping.vert", "shaders/shadowMapping.frag");

	//Used to draw light sphere
	Shader unlitShader("shaders/defaultLit.vert", "shaders/unlit.frag");

	//Used for Post-Processing
	Shader screenShader("shaders/screenShader.vert", "shaders/screenShader.frag");

	//Objects to render in scene
	ew::MeshData cubeMeshData;
	ew::createCube(1.0f, 1.0f, 1.0f, cubeMeshData);
	ew::MeshData sphereMeshData;
	ew::createSphere(0.5f, 128, sphereMeshData);
	ew::MeshData cylinderMeshData;
	ew::createCylinder(1.0f, 0.5f, 64, cylinderMeshData);
	ew::MeshData planeMeshData;
	ew::createPlane(1.0f, 1.0f, planeMeshData);

	ew::Mesh cubeMesh(&cubeMeshData);
	ew::Mesh sphereMesh(&sphereMeshData);
	ew::Mesh planeMesh(&planeMeshData);
	ew::Mesh cylinderMesh(&cylinderMeshData);

	ew::Mesh sceneObjectMeshes[4] = { cubeMesh,planeMesh,cylinderMesh,sphereMesh };

	ew::MeshData unlitSphere;
	ew::createSphere(0.5, 128, unlitSphere);
	ew::Mesh unlitSphereMesh(&unlitSphere);

	//Enable back face culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Initialize shape transforms
	ew::Transform cubeTransform;
	ew::Transform sphereTransform;
	ew::Transform planeTransform;
	ew::Transform cylinderTransform;
	ew::Transform lightTransform;
	ew::Transform pointLightTransform[NUMBER_OF_POINTLIGHTS];

	cubeTransform.position = glm::vec3(-2.0f, 0.0f, 0.0f);
	sphereTransform.position = glm::vec3(0.0f, 0.0f, 0.0f);
	cylinderTransform.position = glm::vec3(2.0f, 0.0f, 0.0f);

	planeTransform.position = glm::vec3(0.0f, -0.5f, 0.0f);
	planeTransform.scale = glm::vec3(10.0f);

	lightTransform.scale = glm::vec3(0.5f);
	lightTransform.position = glm::vec3(0.0f, 5.0f, 0.0f);

	ew::Transform sceneObjectTransforms[4] = { cubeTransform,planeTransform,cylinderTransform,sphereTransform };

	//Initialize material values
	material.mAmbient = glm::vec3(0.25f);
	material.mDiffuse = glm::vec3(0.5f);
	material.mSpecular = glm::vec3(1.0f);
	material.mShininess = float(1.0f);

	//Initialize directional light values
	directionalLight.mColor = glm::vec3(0.75f, 0.75f, 0.75f);
	directionalLight.mDirection = glm::normalize(glm::vec3(1.0f,0.0f,0.5f));
	directionalLight.mIntensity = float(0.5f);

	//Initialize spot light values
	spotLight.mColor = glm::vec3(0.5f, 0.5f, 0.5f);
	spotLight.mDirection = glm::vec3(0.0f, -1.0f, 0.0f);
	spotLight.mIntensity = float(0.5f);
	spotLight.mPosition = lightTransform.position;
	spotLight.mRadius = float(12.0f);
	spotLight.mMinAngle = float(12.5f);
	spotLight.mMaxAngle = float(25.0f);
	spotLight.mFallOff = float(2.0f);

	//Initialize point light values
	pointLights[0].mColor = glm::vec3(1.0f, 0.0f, 0.0f);
	pointLights[0].mIntensity = float(1.0f);
	pointLights[1].mColor = glm::vec3(0.0f, 1.0f, 0.0f);
	pointLights[1].mIntensity = float(1.0f);
	pointLights[2].mColor = glm::vec3(0.0f, 0.0f, 1.0f);
	pointLights[2].mIntensity = float(1.0f);

	//FBO Stuff
	//Create and Bind
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//Create Color Attachemnt for FBO
	GLuint colorBuffer;
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	//Create Depth Buffer
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer is not complete" << std::endl;

	//Create Quad to do Post-Processing on
	ew::MeshData postProcessingQuadData;
	ew::createQuad(2.0f, 2.0f, postProcessingQuadData);
	ew::Mesh postProcessingQuadMesh(&postProcessingQuadData);

	DepthMap shadowMap = createDepthMap(SHADOW_RESOLUTION);

	glm::vec2 effectIntensity = glm::vec2(1.0f,1.0f);

	float minBias = 0.005;
	float maxBias = 0.015;

	//Draw Loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float time = (float)glfwGetTime();
		deltaTime = time - lastFrameTime;
		lastFrameTime = time;

		//update time
		litShader.setFloat("uTime", time);
		litShader.setFloat("uSampleSize", sampleSize);

		//Process material
		processMaterial(litShader, "material", material);

		//Update Lights
		updateLights(time, lightTransform, pointLightTransform);

		//Process Lights
		processAllLights(litShader);
		//Draw first Pass
		//Bind shadow-map
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMap.mFBO);
		//setup viewport
		glViewport(0, 0, SHADOW_RESOLUTION.x, SHADOW_RESOLUTION.y);
		glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Render from directional light's perspective
		shadowShader.use();

		glm::vec3 position = glm::normalize(directionalLight.mDirection) *  8.0f;
		glm::vec3 target = glm::vec3(0.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::mat4 lightView = glm::lookAt(position, target, up);

		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 15.0f);

		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		shadowShader.setMat4("_LightSpaceMatrix", lightSpaceMatrix);
		drawObjects(shadowShader, 4, sceneObjectMeshes, sceneObjectTransforms);

		//Draw second pass
		//Bind fbo
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		//Clear viewport
		glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Draw contents of scene
		litShader.use();
		  
		//bind and set textures
		litShader.setInt("uTexture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		litShader.setInt("uNormalMap", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, NormalMap);
		litShader.setInt("uNoise", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		litShader.setInt("uShadowMap", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, shadowMap.mDepthMap);

		litShader.setMat4("_Projection", camera.getProjectionMatrix());
		litShader.setMat4("_View", camera.getViewMatrix());
		litShader.setVec3("_LightPos", lightTransform.position);
		litShader.setMat4("_LightSpaceMatrix", lightSpaceMatrix);

		litShader.setFloat("uMinBias", minBias);
		litShader.setFloat("uMaxBias", maxBias);

		drawObjects(litShader,4, sceneObjectMeshes, sceneObjectTransforms);

		//Draw light as a small sphere using unlit shader, ironically.
		unlitShader.use();
		unlitShader.setMat4("_Projection", camera.getProjectionMatrix());
		unlitShader.setMat4("_View", camera.getViewMatrix());

		for (int i = 0; i < NUMBER_OF_POINTLIGHTS; i++)
		{
			unlitShader.setVec3("_Color", pointLights[i].mColor);
			drawObject(unlitShader, sphereMesh, pointLightTransform[i]);
		}

		unlitShader.setVec3("_Color", spotLight.mColor);
		drawObject(unlitShader, sphereMesh, lightTransform);

		//Draw Third Pass / apply Screen-Shader Effects
		//Apply Post-Processing Effects to screen-space
		glActiveTexture(GL_TEXTURE4);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glDisable(GL_DEPTH_TEST);

		//use effects shader
		screenShader.use();
		screenShader.setInt("uScreenTexture", 4);
		screenShader.setInt("uEffect", effectIndex);
		screenShader.setVec2("uOffsetDistance", effectIntensity);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		//draw the PP-Quad with the new colorBuffer texture
		postProcessingQuadMesh.draw();

		//IMGui Stuff
		ImGui::Begin("Material");
		ImGui::ColorEdit3("Material Ambient", &material.mAmbient.r);
		ImGui::ColorEdit3("Material Diffuse", &material.mDiffuse.r);
		ImGui::ColorEdit3("Material Specular", &material.mSpecular.r);
		ImGui::DragFloat("Material Shininess", &material.mShininess, 0.01f, 0.0f, 2.0f);
		ImGui::End();

		ImGui::Begin("Directional Light");
		ImGui::ColorEdit3("Light Color", &directionalLight.mColor.r);
		ImGui::DragFloat3("Light Direction", &directionalLight.mDirection.r, 0.1f, -1.0f, 1.0f);
		ImGui::DragFloat("Light Intensity", &directionalLight.mIntensity);
		ImGui::End();

		ImGui::Begin("Spot Light");
		ImGui::ColorEdit3("Light Color", &spotLight.mColor.r);
		ImGui::DragFloat3("Light Direction", &spotLight.mDirection.r, 0.0f, -1.0f, 1.0f);
		ImGui::DragFloat3("Light Position", &lightTransform.position.x, 0.5f, -5.0f, 5.0f);
		ImGui::DragFloat("Light Radius", &spotLight.mRadius, 0.5f, 0.0f, 20.0f);
		ImGui::DragFloat("Light Min Angle", &spotLightMinAngleDegrees, 1.0f, 0.0f, 360.0f);
		ImGui::DragFloat("Light Max Angle", &spotLightMaxAngleDegrees, 1.0f, 0.0f, 360.0f);
		ImGui::DragFloat("Light Angular Falloff", &spotLight.mFallOff, 0.1f, 0.1f, 3.0f);
		ImGui::End();

		ImGui::Begin("Point Lights");
		ImGui::DragFloat3("Orbital Center", &orbitalCenter.x, 1.0f, -5.0f, 5.0f);
		ImGui::DragFloat("Orbital Radius", &orbitalRadius, 1.0f, 1.0f, 10.0f);
		ImGui::DragFloat("Orbital Speed", &orbitalSpeed, 1.0f, 1.0f, 5.0f);
		ImGui::DragFloat("Light Radius", &pointLightRadius, 0.5f, 0.0f, 10.0f);
		ImGui::End();

		ImGui::Begin("Post-Processing");
		ImGui::Combo("Effect", &effectIndex, effectNames, IM_ARRAYSIZE(effectNames));
		ImGui::DragFloat2("Effect Radius", &effectIntensity.r, 0.1f, 0.1f, 8.0f);
		ImGui::End();

		ImGui::Begin("Shadow Bias");
		ImGui::DragFloat("Min Bias", &minBias, 0.001f, 0.001f, 0.5f);
		ImGui::DragFloat("Max Bias", &maxBias, 0.001f, 0.001f, 0.5f);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//drawUI(material, directionalLight, spotLight, lightTransform, orbitalCenter, effectIndex, effectIntensity);

		glfwPollEvents();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
//Author: Eric Winebrenner
void resizeFrameBufferCallback(GLFWwindow* window, int width, int height)
{
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	camera.setAspectRatio((float)SCREEN_WIDTH / SCREEN_HEIGHT);
	glViewport(0, 0, width, height);
}
//Author: Eric Winebrenner
void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
	if (keycode == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	//Reset camera
	if (keycode == GLFW_KEY_R && action == GLFW_PRESS) {
		camera.setPosition(glm::vec3(0, 0, 5));
		camera.setYaw(-90.0f);
		camera.setPitch(0.0f);
		firstMouseInput = false;
	}
	if (keycode == GLFW_KEY_1 && action == GLFW_PRESS) {
		wireFrame = !wireFrame;
		glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);
	}
}
//Author: Eric Winebrenner
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (abs(yoffset) > 0) {
		float fov = camera.getFov() - (float)yoffset * CAMERA_ZOOM_SPEED;
		camera.setFov(fov);
	}
}
//Author: Eric Winebrenner
void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
		return;
	}
	if (!firstMouseInput) {
		prevMouseX = xpos;
		prevMouseY = ypos;
		firstMouseInput = true;
	}
	float yaw = camera.getYaw() + (float)(xpos - prevMouseX) * MOUSE_SENSITIVITY;
	camera.setYaw(yaw);
	float pitch = camera.getPitch() - (float)(ypos - prevMouseY) * MOUSE_SENSITIVITY;
	pitch = glm::clamp(pitch, -89.9f, 89.9f);
	camera.setPitch(pitch);
	prevMouseX = xpos;
	prevMouseY = ypos;
}
//Author: Eric Winebrenner
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	//Toggle cursor lock
	if (button == MOUSE_TOGGLE_BUTTON && action == GLFW_PRESS) {
		int inputMode = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
		glfwSetInputMode(window, GLFW_CURSOR, inputMode);
		glfwGetCursorPos(window, &prevMouseX, &prevMouseY);
	}
}

//Author: Eric Winebrenner
//Returns -1, 0, or 1 depending on keys held
float getAxis(GLFWwindow* window, int positiveKey, int negativeKey) {
	float axis = 0.0f;
	if (glfwGetKey(window, positiveKey)) {
		axis++;
	}
	if (glfwGetKey(window, negativeKey)) {
		axis--;
	}
	return axis;
}

//Author: Eric Winebrenner
//Get input every frame
void processInput(GLFWwindow* window) {

	float moveAmnt = CAMERA_MOVE_SPEED * deltaTime;

	//Get camera vectors
	glm::vec3 forward = camera.getForward();
	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
	glm::vec3 up = glm::normalize(glm::cross(forward, right));

	glm::vec3 position = camera.getPosition();
	position += forward * getAxis(window, GLFW_KEY_W, GLFW_KEY_S) * moveAmnt;
	position += right * getAxis(window, GLFW_KEY_D, GLFW_KEY_A) * moveAmnt;
	position += up * getAxis(window, GLFW_KEY_Q, GLFW_KEY_E) * moveAmnt;
	camera.setPosition(position);
}
//Author: William Box
//Creates a texture from an image-file pointed at by filepath
GLuint createTexture(std::string filepath)
{
	//Texture for this project
	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	//Load texture data from file
	int width, height, numComponents;
	unsigned char* textureData = stbi_load(filepath.c_str(), &width, &height, &numComponents, 0);

	if (numComponents < 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	//Set texture parameters
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(textureData);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}
//Author: William Box
//Creates a null texture for use in post-processing effects
void createColorBuffer(GLuint &colorBuffer)
{
	glGenTextures(1, &colorBuffer);

	glBindTexture(GL_TEXTURE_2D, colorBuffer);

	//Load texture data from file

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}
//Author: William Box
//Creates a buffer with a depth buffer cast to a texture
GLuint createDepthBuffer()
{
	//FBO Stuff
	//Create and Bind
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//generate and bind depth buffer
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo, 0);

	//swap back to default frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fbo;
}

//Author: William Box
DepthMap createDepthMap(glm::vec2 shadowResolution)
{
	DepthMap depthMapOut;
	//FBO Stuff
	//Create and Bind
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//create texture using depth buffer
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowResolution.x, shadowResolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	//attach depth texture to depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	depthMapOut.mFBO = fbo;
	depthMapOut.mDepthMap = depthMap;
	//TODO: Create encapsulation struct for FBO and depthMap;
	return depthMapOut;
}

void updateLights(float time, ew::Transform lightTransform, ew::Transform pointLightTransform[])
{
	spotLight.mPosition = lightTransform.position;
	spotLight.mMinAngle = glm::cos(glm::radians(spotLightMinAngleDegrees));
	spotLight.mMaxAngle = glm::cos(glm::radians(spotLightMaxAngleDegrees));

	for (int i = 0; i < NUMBER_OF_POINTLIGHTS; i++)
	{
		glm::vec3 temp = orbitalCenter;
		temp.x += glm::cos(glm::radians(120.0f * i) + (orbitalSpeed * time)) * orbitalRadius;
		temp.z += glm::sin(glm::radians(120.0f * i) + (orbitalSpeed * time)) * orbitalRadius;
		pointLightTransform[i].position = temp;
	}

	for (int i = 0; i < NUMBER_OF_POINTLIGHTS; i++)
	{
		pointLights[i].mPosition = pointLightTransform[i].position;
		pointLights[i].mRadius = pointLightRadius;
	}
}

void processAllLights(Shader& litShader)
{
	processDirectionalLight(litShader, "dirLight", directionalLight);
	processSpotLight(litShader, "spotLight", spotLight);
	litShader.setInt("numberOfPointLights", NUMBER_OF_POINTLIGHTS);
	for (size_t i = 0; i < NUMBER_OF_POINTLIGHTS; i++)
	{
		processPointLight(litShader, "pointLights", i, pointLights[i]);
	}
}

void drawObjects(Shader& shader,int numOfObjects, ew::Mesh meshes[], ew::Transform transforms[])
{
	for (int i = 0; i < numOfObjects; i++)
	{
		drawObject(shader, meshes[i], transforms[i]);
	}
}

void drawObject(Shader& shader, ew::Mesh& mesh, ew::Transform& transform)
{
	shader.setMat4("_Model", transform.getModelMatrix());
	mesh.draw();
}

//Author: William Box
//void drawUI(Material material, DirLight directionalLight,SpotLight spotLight, ew::Transform lightTransform, glm::vec3 orbitalCenter, int effectIndex, glm::vec2 effectIntensity)
//{
//	//Draw UI
//	ImGui::Begin("Material");
//	ImGui::ColorEdit3("Material Ambient", &material.mAmbient.r);
//	ImGui::ColorEdit3("Material Diffuse", &material.mDiffuse.r);
//	ImGui::ColorEdit3("Material Specular", &material.mSpecular.r);
//	ImGui::DragFloat("Material Shininess", &material.mShininess, 0.01f, 0.0f, 2.0f);
//	ImGui::End();
//
//	ImGui::Begin("Directional Light");
//	ImGui::ColorEdit3("Light Color", &directionalLight.mColor.r);
//	ImGui::DragFloat3("Light Direction", &directionalLight.mDirection.r, 0.1f, 0.0f, 1.0f);
//	ImGui::DragFloat("Light Intensity", &directionalLight.mIntensity);
//	ImGui::End();
//
//	ImGui::Begin("Spot Light");
//	ImGui::ColorEdit3("Light Color", &spotLight.mColor.r);
//	ImGui::DragFloat3("Light Direction", &spotLight.mDirection.r, 0.0f, -1.0f, 1.0f);
//	ImGui::DragFloat3("Light Position", &lightTransform.position.x, 0.5f, -5.0f, 5.0f);
//	ImGui::DragFloat("Light Radius", &spotLight.mRadius, 0.5f, 0.0f, 20.0f);
//	ImGui::DragFloat("Light Min Angle", &spotLightMinAngleDegrees, 1.0f, 0.0f, 360.0f);
//	ImGui::DragFloat("Light Max Angle", &spotLightMaxAngleDegrees, 1.0f, 0.0f, 360.0f);
//	ImGui::DragFloat("Light Angular Falloff", &spotLight.mFallOff, 0.1f, 0.1f, 3.0f);
//	ImGui::End();
//
//	ImGui::Begin("Point Lights");
//	ImGui::DragFloat3("Orbital Center", &orbitalCenter.x, 1.0f, -5.0f, 5.0f);
//	ImGui::DragFloat("Orbital Radius", &orbitalRadius, 1.0f, 1.0f, 10.0f);
//	ImGui::DragFloat("Orbital Speed", &orbitalSpeed, 1.0f, 1.0f, 5.0f);
//	ImGui::DragFloat("Light Radius", &pointLightRadius, 0.5f, 0.0f, 10.0f);
//	ImGui::End();
//
//	ImGui::Begin("Post-Processing");
//	ImGui::Combo("Effect", &effectIndex, effectNames, IM_ARRAYSIZE(effectNames));
//	ImGui::DragFloat2("Effect Intensity", &effectIntensity.r, 0.1f, 0.1f, 8.0f);
//	ImGui::End();
//
//	ImGui::Render();
//	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//}