#pragma once
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace wBox
{
	glm::mat4 scale(float x, float y, float z)
	{
		glm::mat4 scaleMatrix(1);

		scaleMatrix[0][0] = x;
		scaleMatrix[1][1] = y;
		scaleMatrix[2][2] = z;

		return scaleMatrix;

	}

	glm::mat4 scale(glm::vec3 v)
	{
		return scale(v.x, v.y, v.z);
	}

	glm::mat4 rotateX(float a)
	{
		glm::mat4 rotate(1);

		rotate[1][1] = glm::cos(a);
		rotate[1][2] = glm::sin(a);
		rotate[2][1] = -1 * glm::sin(a);
		rotate[2][2] = glm::cos(a);

		return rotate;
	}

	glm::mat4 rotateY(float a)
	{
		glm::mat4 rotate(1);

		rotate[0][0] = glm::cos(a);
		rotate[0][2] = -1 * glm::sin(a);
		rotate[2][0] = glm::sin(a);
		rotate[2][2] = glm::cos(a);

		return rotate;
	}

	glm::mat4 rotateZ(float a)
	{
		glm::mat4 rotate(1);

		rotate[0][0] = glm::cos(a);
		rotate[0][1] = glm::sin(a);
		rotate[1][0] = -1 * glm::sin(a);
		rotate[1][1] = glm::cos(a);

		return rotate;

	}

	glm::mat4 rotateXYZ(glm::vec3 a)
	{
		glm::mat4 rotate(1);
		rotate *= rotateX(a.x);
		rotate *= rotateY(a.y);
		rotate *= rotateZ(a.z);

		return rotate;
	}

	glm::mat4 translate(float x, float y, float z)
	{
		glm::mat4 translate(1);

		translate[3][0] = x;
		translate[3][1] = y;
		translate[3][2] = z;

		return translate;

	}

	glm::mat4 translate(glm::vec3 v)
	{
		return translate(v.x, v.y, v.z);
	}

	struct transform
	{
		glm::vec3 mScale = glm::vec3(1);
		glm::vec3 mRotate = glm::vec3(1);
		glm::vec3 mTranslate = glm::vec3(1);

		glm::mat4 getModelMatrix()
		{
			glm::mat4 temp = glm::mat4(1);

			temp *= translate(mTranslate);
			temp *= rotateXYZ(mRotate);
			temp *= scale(mScale);

			return temp;
		}
	};

	namespace camera
	{
		glm::mat4 lookAt(glm::vec3 eyePos, glm::vec3 targetPos, glm::vec3 up)
		{
			glm::vec3 f = glm::normalize(eyePos - targetPos);
			glm::vec3 r = glm::normalize(glm::cross(up, f));
			glm::vec3 u = glm::normalize(cross(f, r));

			glm::mat4 lookAt(1);
			lookAt[0][0] = r.x;
			lookAt[1][0] = r.y;
			lookAt[2][0] = r.z;
			lookAt[0][1] = u.x;
			lookAt[1][1] = u.y;
			lookAt[2][1] = u.z;
			lookAt[0][2] = f.x;
			lookAt[1][2] = f.y;
			lookAt[2][2] = f.z;

			glm::vec3 translationVector = eyePos;

			translationVector *= -1;

			lookAt *= translate(translationVector);

			return lookAt;
		}

		glm::mat4 ortho(float left, float right, float bottom, float top, float nearPlane, float farPlane)
		{
			glm::mat4 orthoMatrix(1);
			orthoMatrix[0][0] = 2 / (right - left);
			orthoMatrix[1][1] = 2 / (top - bottom);
			orthoMatrix[2][2] = -2 / (farPlane - nearPlane);
			orthoMatrix[3][0] = (-1 * (right + left)) / (right - left);
			orthoMatrix[3][1] = (-1 * (top + bottom)) / (top - bottom);
			orthoMatrix[3][2] = (-1 * (farPlane + nearPlane)) / (farPlane - nearPlane);

			return orthoMatrix;
		}

		glm::mat4 ortho(float height, float aspectRatio, float nearPlane, float farPlane)
		{
			float bottom = height / -2;
			float top = height / 2;

			float width = aspectRatio * height;

			float left = width / -2;
			float right = width / 2;

			return ortho(left, right, bottom, top, nearPlane, farPlane);
		}
		glm::mat4 perspective(float fov, float aspectRatio, float nearPlane, float farPlane)
		{
			float fovRad = glm::radians(fov);
			float c = glm::tan(fovRad / 2);
			glm::mat4 perspectiveMatrix(1);
			perspectiveMatrix[0][0] = 1 / (aspectRatio * c);
			perspectiveMatrix[1][1] = 1 / c;
			perspectiveMatrix[2][2] = -1 * ((farPlane + nearPlane) / (farPlane - nearPlane));
			perspectiveMatrix[2][3] = -1;
			perspectiveMatrix[3][2] = -1 * ((2 * farPlane * nearPlane) / (farPlane - nearPlane));

			return perspectiveMatrix;
		}

		struct camera
		{

		};
	}
}