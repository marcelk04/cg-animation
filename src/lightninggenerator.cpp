#include "lightninggenerator.hpp"

#include "framework/common.hpp"

#include <list>
#include <cstdlib>
#include <iostream>

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
	return os << '[' << vec.x << ", " << vec.y << ", " << vec.z << ']';
}

std::vector<LightningGenerator::Segment> LightningGenerator::genBolt(glm::vec3 startpoint, glm::vec3 endpoint, int generations, const glm::vec3& camDir) {
	std::list<Segment> segmentList;
	Segment s{ startpoint, endpoint };
	segmentList.push_back(s);

	float offsetAmount = 0.2f;
	float offshootOffsetAmount = 0.75f * offsetAmount;
	float lengthScale = 0.7f;
	float offshotChance = 0.3f;

	for (int g = 0; g < generations; g++) {
		std::list<Segment> newSegments;

		for (Segment seg : segmentList) {
			glm::vec3 midpoint = 0.5f * (seg.startpoint + seg.endpoint);
			glm::vec3 direction = seg.endpoint - seg.startpoint;
			glm::vec3 offsetDir = glm::normalize(glm::cross(direction, camDir));

			float randomOffset = Common::randomFloat(-offsetAmount, offsetAmount);

            midpoint += randomOffset * offsetDir;

			Segment s1{ seg.startpoint, midpoint };
			Segment s2{ midpoint, seg.endpoint };

			newSegments.push_back(s1);

			// generate offshoots
			if (Common::randomFloat() < offshotChance) {
				float offshootOffset = Common::randomFloat(-offshootOffsetAmount, offshootOffsetAmount);

				glm::vec3 offshootDir = midpoint - seg.startpoint;
				glm::vec3 offshootEnd = (midpoint + offshootDir * lengthScale) + (offshootOffset * offsetDir);
				Segment s3{ midpoint, offshootEnd };
				newSegments.push_back(s3);
			}
			
			newSegments.push_back(s2);
		}

		offsetAmount /= 2.0f;
		offshootOffsetAmount /= 2.0f;	

		segmentList = newSegments;
	}

	return std::vector<Segment>(segmentList.begin(), segmentList.end());
}

LightningGenerator::MeshData LightningGenerator::genMeshData(const std::vector<Segment>& segments, const glm::vec3& camDir) {
	float halfwidth = 0.005f;

	std::vector<Mesh::VertexPCN> vertices(segments.size() * 4);
	std::vector<unsigned int> indices(segments.size() * 6);

	glm::vec3 startpoint = segments[0].startpoint;
	glm::vec3 endpoint = segments[segments.size()-1].endpoint;
	glm::vec3 offsetVec = glm::normalize(glm::cross(endpoint - startpoint, camDir));

	int vIdx = 0;
	int iIdx = 0;
	int vCount = 0;
	for (const Segment& seg : segments) {
		Mesh::VertexPCN v0{ seg.startpoint - halfwidth * offsetVec, glm::vec2(0.0f, 0.0f), -camDir };
		Mesh::VertexPCN v1{ seg.startpoint + halfwidth * offsetVec, glm::vec2(0.0f, 1.0f), -camDir };
		Mesh::VertexPCN v2{ seg.endpoint - halfwidth * offsetVec, glm::vec2(1.0f, 0.0f), -camDir };
		Mesh::VertexPCN v3{ seg.endpoint + halfwidth * offsetVec, glm::vec2(1.0f, 1.0f), -camDir };

		vertices[vIdx++] = v0;
		vertices[vIdx++] = v1;
		vertices[vIdx++] = v2;
		vertices[vIdx++] = v3;

		// v0 - v1
		// |  /  |
		// v2 - v3
		//
		// Indices: 0 2 1 1 2 3

		indices[iIdx++] = vCount + 0;
		indices[iIdx++] = vCount + 2;
		indices[iIdx++] = vCount + 1;
		indices[iIdx++] = vCount + 1;
		indices[iIdx++] = vCount + 2;
		indices[iIdx++] = vCount + 3;

		vCount += 4;
	}

	return MeshData(vertices, indices);
}