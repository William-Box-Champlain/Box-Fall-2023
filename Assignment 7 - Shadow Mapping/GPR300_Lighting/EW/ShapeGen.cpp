//Author: Eric Winebrenner

#include "ShapeGen.h"
#include <glm/gtc/type_ptr.hpp>

namespace ew {
	void createPlane(float width, float height, MeshData& meshData) {
		meshData.vertices.clear();
		meshData.indices.clear();
		float halfWidth = width / 2.0f;
		float halfHeight = height / 2.0f;
		Vertex vertices[4] = {
			//Front face
			{glm::vec3(-halfWidth, 0, -halfHeight), glm::vec3(0,1,0), glm::vec3(1,0,0), glm::vec2(0,0)}, //BL
			{glm::vec3(+halfWidth, 0, -halfHeight), glm::vec3(0,1,0), glm::vec3(1,0,0), glm::vec2(1,0)}, //BR
			{glm::vec3(+halfWidth, 0, +halfHeight), glm::vec3(0,1,0), glm::vec3(1,0,0), glm::vec2(1,1)}, //TR
			{glm::vec3(-halfWidth, 0, +halfHeight), glm::vec3(0,1,0), glm::vec3(1,0,0), glm::vec2(0,1)} //TL
		};
		meshData.vertices.assign(&vertices[0], &vertices[4]);
		unsigned int indices[6] = {
			// front face
			0, 2, 1,
			0, 3, 2
		};
		meshData.indices.assign(&indices[0], &indices[6]);
	};

	void createQuad(float width, float height, MeshData& meshData) {
		meshData.vertices.clear();
		meshData.indices.clear();
		float halfWidth = width / 2.0f;
		float halfHeight = height / 2.0f;
		Vertex vertices[4] = {
			//Front face
			{glm::vec3(-halfWidth, -halfHeight, 0), glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec2(0,0)}, //BL
			{glm::vec3(+halfWidth, -halfHeight, 0), glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec2(1,0)}, //BR
			{glm::vec3(+halfWidth, +halfHeight, 0), glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec2(1,1)}, //TR
			{glm::vec3(-halfWidth, +halfHeight, 0), glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec2(0,1)} //TL
		};
		meshData.vertices.assign(&vertices[0], &vertices[4]);
		unsigned int indices[6] = {
			// front face
			0, 1, 2,
			0, 2, 3
		};
		meshData.indices.assign(&indices[0], &indices[6]);
	};

	void createCube(float width, float height, float depth, MeshData& meshData)
	{
		meshData.vertices.clear();
		meshData.indices.clear();

		float halfWidth = width / 2.0f;
		float halfHeight = height / 2.0f;
		float halfDepth = depth / 2.0f;

		//VERTICES
		//-------------
		Vertex vertices[24] = {
			//Front face
			{glm::vec3(-halfWidth, -halfHeight, +halfDepth), glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec2(0,0)}, //BL
			{glm::vec3(+halfWidth, -halfHeight, +halfDepth), glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec2(1,0)}, //BR
			{glm::vec3(+halfWidth, +halfHeight, +halfDepth), glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec2(1,1)}, //TR
			{glm::vec3(-halfWidth, +halfHeight, +halfDepth), glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec2(0,1)}, //TL

			//Back face
			{glm::vec3(+halfWidth, -halfHeight, -halfDepth), glm::vec3(0,0,-1), glm::vec3(-1,0,0), glm::vec2(0,0)}, //BL
			{glm::vec3(-halfWidth, -halfHeight, -halfDepth), glm::vec3(0,0,-1), glm::vec3(-1,0,0), glm::vec2(1,0)}, //BR
			{glm::vec3(-halfWidth, +halfHeight, -halfDepth), glm::vec3(0,0,-1), glm::vec3(-1,0,0), glm::vec2(1,1)}, //TR
			{glm::vec3(+halfWidth, +halfHeight, -halfDepth), glm::vec3(0,0,-1), glm::vec3(-1,0,0), glm::vec2(0,1)}, //TL

			//Right face
			{glm::vec3(+halfWidth, -halfHeight, +halfDepth), glm::vec3(1,0,0), glm::vec3(0,0,1), glm::vec2(1,0)}, //BL
			{glm::vec3(+halfWidth, -halfHeight, -halfDepth), glm::vec3(1,0,0), glm::vec3(0,0,1), glm::vec2(0,0)}, //BR
			{glm::vec3(+halfWidth, +halfHeight, -halfDepth), glm::vec3(1,0,0), glm::vec3(0,0,1), glm::vec2(0,1)}, //TR
			{glm::vec3(+halfWidth, +halfHeight, +halfDepth), glm::vec3(1,0,0), glm::vec3(0,0,1), glm::vec2(1,1)}, //TL

			//Left face
			{glm::vec3(-halfWidth, -halfHeight, -halfDepth), glm::vec3(-1,0,0), glm::vec3(0,0,-1), glm::vec2(1,0)}, //BL
			{glm::vec3(-halfWidth, -halfHeight, +halfDepth), glm::vec3(-1,0,0), glm::vec3(0,0,-1), glm::vec2(0,0)}, //BR
			{glm::vec3(-halfWidth, +halfHeight, +halfDepth), glm::vec3(-1,0,0), glm::vec3(0,0,-1), glm::vec2(0,1)}, //TR
			{glm::vec3(-halfWidth, +halfHeight, -halfDepth), glm::vec3(-1,0,0), glm::vec3(0,0,-1), glm::vec2(1,1)}, //TL

			//Top face
			{glm::vec3(-halfWidth, +halfHeight, +halfDepth), glm::vec3(0,1,0), glm::vec3(1,0,0), glm::vec2(0,1)}, //BL
			{glm::vec3(+halfWidth, +halfHeight, +halfDepth), glm::vec3(0,1,0), glm::vec3(1,0,0), glm::vec2(1,1)}, //BR
			{glm::vec3(+halfWidth, +halfHeight, -halfDepth), glm::vec3(0,1,0), glm::vec3(1,0,0), glm::vec2(1,0)}, //TR
			{glm::vec3(-halfWidth, +halfHeight, -halfDepth), glm::vec3(0,1,0), glm::vec3(1,0,0), glm::vec2(0,0)}, //TL

			//Bottom face
			{glm::vec3(-halfWidth, -halfHeight, -halfDepth), glm::vec3(0,-1,0), glm::vec3(-1,0,0), glm::vec2(0,1)}, //BL
			{glm::vec3(+halfWidth, -halfHeight, -halfDepth), glm::vec3(0,-1,0), glm::vec3(-1,0,0), glm::vec2(1,1)}, //BR
			{glm::vec3(+halfWidth, -halfHeight, +halfDepth), glm::vec3(0,-1,0), glm::vec3(-1,0,0), glm::vec2(1,0)}, //TR
			{glm::vec3(-halfWidth, -halfHeight, +halfDepth), glm::vec3(0,-1,0), glm::vec3(-1,0,0), glm::vec2(0,0)}, //TL
		};
		meshData.vertices.assign(&vertices[0], &vertices[24]);

		//INDICES
		//-------------

		unsigned int indices[36] = {
			// front face
			0, 1, 2,
			0, 2, 3,

			// back face
			4, 5, 6,
			6, 7, 4,

			// right face
			8,  9, 10,
			10, 11, 8,

			//left face 
			12, 13, 14,
			14, 15, 12,

			//top face
			16,17,18,
			18,19,16,

			//bottom face
			20, 21, 22,
			22, 23, 20
		};
		meshData.indices.assign(&indices[0], &indices[36]);
	}

	void createSphere(float radius, int numSegments, MeshData& meshData)
	{
		meshData.vertices.clear();
		meshData.indices.clear();

		float topY = radius;
		float bottomY = -radius;
		unsigned int topIndex = 0;
		meshData.vertices.push_back({ glm::vec3(0,topY,0),glm::vec3(0,1,0), glm::vec3(1,0,0), glm::vec2(0.5f,0.5f)});

		//Angle between segments
		float thetaStep = (2.0f * glm::pi<float>()) / (float)numSegments;
		float phiStep = (glm::pi<float>()) / (float)numSegments;
		float circumference = glm::pi<float>() * 2.0f * radius;

		for (int i = 1; i < numSegments; i++)
		{
			float phi = phiStep * i;

			//Create row
			for (int j = 0; j <= numSegments; ++j)
			{
				float theta = thetaStep * j;

				float x = radius * sinf(phi) * sinf(theta);
				float y = radius * cosf(phi);
				float z = radius * sinf(phi) * cosf(theta);

				float xArcLength = theta * radius;
				float yArcLength = phi * radius;

				glm::vec3 position = glm::vec3(x, y, z);
				glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));

				//TODO: Calculate proper tangent direction
				glm::vec3 tangent = glm::normalize(glm::cross(normal,glm::vec3(0.0f,1.0f,0.0f)));

				float u = xArcLength / circumference;
				float v = yArcLength / circumference;

				glm::vec2 uv = glm::vec2(u,v);
				meshData.vertices.push_back({ position, normal , tangent , uv});
			}
		}
		meshData.vertices.push_back({ glm::vec3(0,bottomY,0), glm::vec3(0,-1,0), glm::vec3(-1,0,0), glm::vec2(0.5f,0.5f) });
		unsigned int bottomIndex = (unsigned int)meshData.vertices.size() - 1;
		unsigned int ringVertexCount = numSegments + 1;

		//TOP CAP
		for (int i = 0; i < numSegments; ++i) {
			meshData.indices.push_back(topIndex); //top cap center 
			meshData.indices.push_back(i + 1);
			meshData.indices.push_back(i + 2);
		}

		//RINGS
		unsigned int start = 1;

		//Row index
		//-2 to ignore poles
		for (int y = 0; y < numSegments - 2; ++y)
		{
			//Column index
			for (int x = 0; x < numSegments; ++x)
			{
				//Triangle 1
				meshData.indices.push_back(start + y * ringVertexCount + x);
				meshData.indices.push_back(start + (y + 1) * ringVertexCount + x);
				meshData.indices.push_back(start + y * ringVertexCount + x + 1);

				//Triangle 2
				meshData.indices.push_back(start + y * ringVertexCount + x + 1);
				meshData.indices.push_back(start + (y + 1) * ringVertexCount + x);
				meshData.indices.push_back(start + (y + 1) * ringVertexCount + x + 1);
			}
		}

		start = bottomIndex - ringVertexCount;

		//BOTTOM CAP
		for (unsigned int i = 0; i < ringVertexCount; ++i) {
			meshData.indices.push_back(start + i + 1);
			meshData.indices.push_back(start + i);
			meshData.indices.push_back(bottomIndex); //bottom cap center 
		}
	}

	void createCylinder(float height, float radius, int numSegments, MeshData& meshData)
	{
		meshData.vertices.clear();
		meshData.indices.clear();

		float halfHeight = height * 0.5f;
		float thetaStep = glm::pi<float>() * 2.0f / numSegments;
		float circumference = glm::pi<float>() * 2.0f * radius;

		//VERTICES
		//Top cap (facing up)
		meshData.vertices.push_back(Vertex(glm::vec3(0, halfHeight, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0),glm::vec2(0.0f,0.0f)));
		for (int i = 0; i <= numSegments; i++)
		{
			glm::vec3 pos = glm::vec3(
				cos(i * thetaStep) * radius,
				halfHeight,
				sin(i * thetaStep) * radius
			);
			float u = cos(i * thetaStep);
			float v = sin(i * thetaStep);
			glm::vec2 uv = glm::vec2(u, v);
			meshData.vertices.push_back(Vertex(pos, glm::vec3(0, 1, 0), glm::vec3(1, 0, 0),uv));
		}

		//Bottom cap (facing down)
		meshData.vertices.push_back(Vertex(glm::vec3(0, -halfHeight, 0), glm::vec3(0, -1, 0), glm::vec3(1, 0, 0),glm::vec2(0.0f,0.0f)));
		unsigned int bottomCenterIndex = (unsigned int)meshData.vertices.size() - 1;
		for (int i = 0; i <= numSegments; i++)
		{
			glm::vec3 pos = glm::vec3(
				cos(i * thetaStep) * radius,
				-halfHeight,
				sin(i * thetaStep) * radius
			);
			float u = cos(i * thetaStep);
			float v = sin(i * thetaStep);
			glm::vec2 uv = glm::vec2(u, v);
			meshData.vertices.push_back(Vertex(pos, glm::vec3(0, -1, 0), glm::vec3(1, 0, 0),uv));
		}

		//Sides (facing out)
		unsigned int sideStartIndex = (unsigned int)meshData.vertices.size();
		//Side top ring
		for (int i = 0; i <= numSegments; i++)
		{
			float arcLength = i * thetaStep * radius;
			glm::vec3 pos = meshData.vertices[i + 1].position;
			glm::vec3 normal = glm::normalize((pos - meshData.vertices[0].position));
			glm::vec3 tangent = glm::normalize(cross(normal,glm::vec3(0.0f,1.0f,0.0f)));
			float u = arcLength/ circumference;
			float v = pos.y;
			glm::vec2 uv = glm::vec2(u, v);
			meshData.vertices.push_back(Vertex(pos, normal, tangent, uv));
		}
		//Side bottom ring
		for (int i = 0; i <= numSegments; i++)
		{
			float arcLength = i * thetaStep * radius;
			glm::vec3 pos = meshData.vertices[bottomCenterIndex + i + 1].position;
			glm::vec3 normal = glm::normalize((pos - meshData.vertices[bottomCenterIndex].position));
			glm::vec3 tangent = glm::normalize(cross(normal, glm::vec3(0.0f, 1.0f, 0.0f)));
			float u = arcLength/ circumference;
			float v = pos.y;
			glm::vec2 uv = glm::vec2(u, v);
			meshData.vertices.push_back(Vertex(pos, normal, tangent, uv));
		}

		//INDICES
		//Top cap
		for (int i = 0; i < numSegments; i++)
		{
			meshData.indices.push_back(i + 1);
			meshData.indices.push_back(0);
			meshData.indices.push_back(i + 2);
		}
		//Bottom cap
		for (int i = 0; i < numSegments; i++)
		{
			meshData.indices.push_back(bottomCenterIndex);
			meshData.indices.push_back(bottomCenterIndex + i + 1);
			meshData.indices.push_back(bottomCenterIndex + i + 2);
		}
		//Side quads
		for (int i = 0; i < numSegments; i++)
		{
			unsigned int start = sideStartIndex + i;
			meshData.indices.push_back(start);
			meshData.indices.push_back(start + 1);
			meshData.indices.push_back(start + numSegments + 1);
			meshData.indices.push_back(start + numSegments + 1);
			meshData.indices.push_back(start + 1);
			meshData.indices.push_back(start + numSegments + 2);
		}
	}

}