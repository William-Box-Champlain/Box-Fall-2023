#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "EW/Shader.h"

#include <string>
#include <sstream>
#include <iostream>

struct Material
{
    glm::vec3 mAmbient;
    glm::vec3 mDiffuse;
    glm::vec3 mSpecular;
    float mShininess;
};

struct SpotLight
{
    glm::vec3 mColor;

    glm::vec3 mPosition;

    glm::vec3 mDirection;

    float mIntensity;
    float mRadius;

    float mMinAngle;
    float mMaxAngle;
};

struct PointLight
{
    glm::vec3 mPosition;

    glm::vec3 mColor;

    float mIntensity;

    float mRadius;

};

struct DirLight
{
    glm::vec3 mDirection;

    glm::vec3 mColor;

    float mIntensity;
};

void processMaterial(Shader& shader, std::string materialName, Material material)
{
	std::stringstream ss0;
	ss0 << materialName;
	shader.setVec3(ss0.str() + ".mAmbient", material.mAmbient);
	shader.setVec3(ss0.str() + ".mDiffuse", material.mDiffuse);
	shader.setVec3(ss0.str() + ".mSpecular", material.mSpecular);
	shader.setFloat(ss0.str() + ".mShininess", material.mShininess);

}

void processDirectionalLight(Shader& shader, std::string lightName, DirLight light)
  {
	std::stringstream ss0;
	ss0 << lightName;
	shader.setVec3(ss0.str() + ".mColor", light.mColor);
	shader.setVec3(ss0.str() + ".mDirection", light.mDirection);
	shader.setFloat(ss0.str() + ".mIntensity", light.mIntensity);
}

void processSpotLight(Shader& shader, std::string lightName, SpotLight light)
{
	std::stringstream ss0;
	ss0 << lightName;
	shader.setVec3(ss0.str() + ".mPosition", light.mPosition);
	shader.setVec3(ss0.str() + ".mDirection", light.mDirection);
	shader.setVec3(ss0.str() + ".mColor", light.mColor);
	shader.setFloat(ss0.str() + ".mIntensity", light.mIntensity);
	shader.setFloat(ss0.str() + ".mRadius", light.mRadius);
	shader.setFloat(ss0.str() + ".mMinAngle", light.mMinAngle);
	shader.setFloat(ss0.str() + ".mMaxAngle", light.mMaxAngle);
	
}

void processPointLight(Shader& shader, std::string lightName, int lightNum, PointLight light)
{
	std::stringstream ss0, ss1, ss2;

	ss0 << lightName;
	ss1 << lightNum;
	ss2 << ss0.str() + "[" + ss1.str() + "]";

	shader.setVec3(ss2.str() + ".mPosition", light.mPosition);
	shader.setVec3(ss2.str() + ".mColor", light.mColor);
	shader.setFloat(ss2.str() + ".mIntensity", light.mIntensity);
	shader.setFloat(ss2.str() + ".mRadius", light.mRadius);
}

void outputMaterialValues(Material material)
{
	std::cout << "Material Ambient: " << material.mAmbient.x << ", " << material.mAmbient.y << ", " << material.mAmbient.z << std::endl;
	std::cout << "Material Diffuse: " << material.mDiffuse.x << ", " << material.mDiffuse.y << ", " << material.mDiffuse.z << std::endl;
	std::cout << "Material Specular: " << material.mSpecular.x << ", " << material.mSpecular.y << ", " << material.mSpecular.z << std::endl;
	std::cout << "Material Shininess: " << material.mShininess << std::endl;
}

void outputDirLightValues(DirLight light)
{
	std::cout << "Directional Light Values" << std::endl;
	std::cout << "Light Direction: " << light.mDirection.x << ", " << light.mDirection.y << ", " << light.mDirection.z << std::endl;
	std::cout << "Light Color: " << light.mColor.x << ", " << light.mColor.y << ", " << light.mColor.z << std::endl;
	std::cout << "Light Intensity: " << light.mIntensity << std::endl;
}

void outputSpotLightValues(SpotLight light)
{
	std::cout << "Spot Light Values" << std::endl;
	std::cout << "Light Position: " << light.mPosition.x << ", " << light.mPosition.y << ", " << light.mPosition.z << std::endl;
	std::cout << "Light Direction: " << light.mDirection.x << ", " << light.mDirection.y << ", " << light.mDirection.z << std::endl;
	std::cout << "Light Color: " << light.mColor.x << ", " << light.mColor.y << ", " << light.mColor.z << std::endl;
	std::cout << "Light Intensity: " << light.mIntensity << std::endl;
	std::cout << "Light Radius: " << light.mRadius << std::endl;
	std::cout << "Light min Angle: " << light.mMinAngle << std::endl;
	std::cout << "Light max Angle: " << light.mMaxAngle << std::endl;
}

void outputPointLightValues(PointLight light)
{
	std::cout << "Point Light Values" << std::endl;
	std::cout << "Light Position: " << light.mPosition.x << ", " << light.mPosition.y << ", " << light.mPosition.z << std::endl;
	std::cout << "Light Color: " << light.mColor.x << ", " << light.mColor.y << ", " << light.mColor.z << std::endl;
	std::cout << "Light Intensity: " << light.mIntensity << std::endl;
	std::cout << "Light Radius: " << light.mRadius << std::endl;
}