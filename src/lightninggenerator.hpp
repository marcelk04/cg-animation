#pragma once

#include <glm/glm.hpp>

#include "framework/mesh.hpp"

#include <vector>

class LightningGenerator {
public:
	using MeshData = std::pair<std::vector<Mesh::VertexPCN>, std::vector<unsigned int>>;

	struct Segment {
		glm::vec3 startpoint;
		glm::vec3 endpoint;
	};

	static std::vector<Segment> genBolt(glm::vec3 startpoint, glm::vec3 endpoint, int generations, const glm::vec3& camDir);
	static std::vector<float> getVertices(const std::vector<Segment>& segments);
	static MeshData genMeshData(const std::vector<Segment>& segments, const glm::vec3& camDir);
};